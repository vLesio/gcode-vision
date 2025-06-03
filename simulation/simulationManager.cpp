#include "SimulationManager.h"
#include <iostream>
#include <condition_variable>
#include "primitives.h"
#include "scene.h"

bool SimulationManager::initializeSimulation(
    const std::string& gcodeFile,
    const std::string& printerName,
    float nozzleDiameter,
	float maxSpeed,
    float speed)
{
    context.clear();
    context.loadedFilename = gcodeFile;
    context.printerName = printerName;
    context.nozzleDiameter = nozzleDiameter;
	context.maxSpeed = maxSpeed;
    context.simulationSpeed = speed;
    context.simulationScale = 0.01f;

    if (!loadGCode(gcodeFile))
        return false;

    state = SimulationState::Initialized;
    return true;
}

void SimulationManager::prepareSimulationScene(Scene* scene) {
    if (state != SimulationState::Initialized) {
        std::cerr << "[SimulationManager] Cannot prepare scene. Initialize first.\n";
        return;
    }

	printEngine.reset();
	printheadAnimator.reset();

    // Initialize filament objects
    //context.filamentObject = Primitives::createDirectionalCube();
	context.filamentObject = Primitives::createDirectedCylinder(16, 0.5f, 1.0f);
    context.filamentObject->reserveInstances(context.printSteps.size() + 100);

    //context.tempSegmentObject = Primitives::createDirectionalCube();
    context.tempSegmentObject = Primitives::createDirectedCylinder(16, 0.5f, 1.0f);
    context.tempSegmentObject->reserveInstances(1);

	assert(context.filamentObject && "Filament object not created!");
	assert(context.tempSegmentObject && "Temporary segment object not created!");

    scene->addInstanced(context.filamentObject);
    scene->addInstanced(context.tempSegmentObject);

	printEngine = std::make_unique<PrintSimulationEngine>();
	printEngine->attachObjects(context.filamentObject, context.tempSegmentObject);

	// Initialize print-head animator
    if (!context.printSteps.empty()) {
        printheadAnimator = std::make_unique<PrintheadAnimator>(context.simulationScale, context.printSteps[0].startPosition);
        printheadAnimator->attachPrinter(printer);
        printheadAnimator->initialize();
        printheadAnimator->attachToScene(scene);
    }

    state = SimulationState::Prepared;
}

void SimulationManager::beginSimulation() {
    if (state != SimulationState::Prepared) {
        std::cerr << "[SimulationManager] Cannot begin. Scene not prepared.\n";
        return;
    }

    if (printEngine)
        printEngine->reset();

    context.simulationTime = 0.0f;
    context.currentStepIndex = 0;
    timer.reset();
    state = SimulationState::Running;

    std::cout << "[SimulationManager] Simulation started.\n";
}

void SimulationManager::pauseSimulation() {
    if (state == SimulationState::Running) {
        timer.pause();
        state = SimulationState::Paused;
        std::cout << "[SimulationManager] Simulation paused.\n";
    }
}

void SimulationManager::resumeSimulation() {
    if (state == SimulationState::Paused) {
        timer.resume();
        state = SimulationState::Running;
        std::cout << "[SimulationManager] Simulation resumed.\n";
    }
}

void SimulationManager::stepForward() {
	if (!isStateOneOf(state, { SimulationState::Paused})) {
        std::cerr << "[SimulationManager] Cannot step forward unless paused\n";
        return;
    }

    if (context.currentStepIndex >= context.printSteps.size()) return;

    const PrintStep& step = context.printSteps[context.currentStepIndex];
    printEngine->simulateStep(context, step);

    if (printheadAnimator)
        printheadAnimator->step(step);

    context.currentStepIndex++;
    context.simulationTime = 0.0f;

    std::cout << "[SimulationManager] Step forward: " << context.currentStepIndex << "\n";
}

void SimulationManager::stepBackward() {
    if (!isStateOneOf(state, { SimulationState::Paused})) {
        std::cerr << "[SimulationManager] Cannot step backwards unless paused\n";
        return;
    }

    if (context.currentStepIndex > 0) {
        context.currentStepIndex--;
        printEngine->simulateStepsInRange(context, context.currentStepIndex);
        context.simulationTime = 0.0f;

        if (printheadAnimator)
			printheadAnimator->step(context.printSteps[context.currentStepIndex]);

        std::cout << "[SimulationManager] Step backward: " << context.currentStepIndex << "\n";
    }
}

void SimulationManager::resetSimulation() {
    context.resetRuntime();
    timer.pause();

    if (printheadAnimator) {
        printheadAnimator->reset();
    }

	if (printEngine) {
		printEngine->reset();
	}

    state = SimulationState::Prepared;
    std::cout << "[SimulationManager] Simulation reset.\n";
}

void SimulationManager::tickSimulation() {
    if (state != SimulationState::Running || !printEngine)
        return;

    float deltaTime = timer.tick();
    if (deltaTime <= 0.0f) return;

    context.simulationTime += deltaTime * context.simulationSpeed;

    while (context.currentStepIndex < context.printSteps.size()) {
        const PrintStep& step = context.printSteps[context.currentStepIndex];
        float moveLength = glm::distance(step.startPosition, step.endPosition);
		float speed = (step.speed > 0.0f) ? step.speed : context.maxSpeed; // Default speed if not specified in step
        float moveDuration = moveLength / (speed / 60.0f);

        float progress = glm::clamp(context.simulationTime / moveDuration, 0.0f, 1.0f);

        if (progress < 1.0f) {
            printEngine->simulatePartialStep(context, step, progress);
            if (printheadAnimator)
                printheadAnimator->updateInterpolated(step.startPosition, step.endPosition, progress);
            return;
        }

        printEngine->simulateStep(context, step);

        if (printheadAnimator)
            printheadAnimator->update(step.endPosition);

        context.simulationTime -= moveDuration;
        context.currentStepIndex++;

        if (context.currentStepIndex >= context.printSteps.size()) {
            state = SimulationState::Completed;
            std::cout << "[SimulationManager] Simulation completed.\n";
            break;
        }
    }
}

void SimulationManager::handleTerminate() {
    {
        std::lock_guard<std::mutex> lock(terminateMutex);
        terminateDone = false;
    }

    printheadAnimator.reset();
	printEngine.reset();

    if (context.filamentObject && scenePtr)
        scenePtr->removeInstanced(context.filamentObject);
    if (context.tempSegmentObject && scenePtr)
        scenePtr->removeInstanced(context.tempSegmentObject);

    context.filamentObject = nullptr;
    context.tempSegmentObject = nullptr;

    context.clear();
    state = SimulationState::Uninitialized;

    {
        std::lock_guard<std::mutex> lock(terminateMutex);
        terminateDone = true;
    }
    terminateCV.notify_one();

    std::cout << "[SimulationManager] Simulation terminated.\n";
}


bool SimulationManager::tryStartSimulation(Scene* scene) {
    switch (state) {
    case SimulationState::Initialized:
        prepareSimulationScene(scene);
        //fallthrough
    case SimulationState::Prepared:
        beginSimulation();
        return true;
    case SimulationState::Paused:
        resumeSimulation();
        return true;
    case SimulationState::Completed:
        resetSimulation();
        beginSimulation();
        return true;
    default:
        std::cerr << "[SimulationManager] Cannot start simulation in current state.\n";
        return false;
    }
}

void SimulationManager::enqueueEvent(SimulationEvent event) {
    std::lock_guard<std::mutex> lock(eventMutex);
    eventQueue.push(event);
}

void SimulationManager::processEvents() {
    std::lock_guard<std::mutex> lock(eventMutex);
    while (!eventQueue.empty()) {
        SimulationEvent ev = eventQueue.front();
        eventQueue.pop();

        switch (ev) {
        case SimulationEvent::StepForward: stepForward(); break;
        case SimulationEvent::StepBackward: stepBackward(); break;
        case SimulationEvent::Pause: pauseSimulation(); break;
        case SimulationEvent::Resume: resumeSimulation(); break;
        case SimulationEvent::Reset: resetSimulation(); break;
        case SimulationEvent::Begin:
            if (scenePtr) tryStartSimulation(scenePtr);
            break;
        case SimulationEvent::Terminate:
            handleTerminate();
            break;
        case SimulationEvent::ChangeSpeed: updateSpeed(); break;
        }
    }
}

SimulationManager& SimulationManager::get() {
    static SimulationManager instance;
    return instance;
}

bool SimulationManager::loadGCode(const std::string& path) {
    context.loadedFilename = path;
    GCodeParser parser;
    if (!parser.loadFile(path)) {
        std::cerr << "[SimulationManager] Failed to load G-code file: " << path << std::endl;
        return false;
    }
    context.printSteps = parser.getPrintSteps();
    context.printStepsCount = context.printSteps.size();
    return true;
}


void SimulationManager::simulateFullPrint() {
    if (state != SimulationState::Prepared || !printEngine) {
        std::cerr << "[SimulationManager] Cannot simulate full print.\n";
        return;
    }

    printEngine->simulate(context);
    state = SimulationState::Completed;
}

const SimulationContext& SimulationManager::getContext() const {
    return context;
}

SimulationContext& SimulationManager::getContextMutable() {
    return context;
}

SimulationState SimulationManager::getState() const {
    return state;
}

bool SimulationManager::wasAlreadySimulated() const {
    return state == SimulationState::Completed;
}

void SimulationManager::setPrinter(std::shared_ptr<Printer> incomingPrinter)
{
    printer = incomingPrinter;
}

void SimulationManager::setSpeed(float speed)
{
    if (speed < 0.0f)
    {
        std::cerr << "[SimulationManager] Invalid speed value: " << speed << ". Must be between 0.0 and 1000.0.\n";
        speed = 1.0f;
    }
    else if (speed > 1000.0f)
    {
        std::cerr << "[SimulationManager] Invalid speed value: " << speed << ". Must be between 0.0 and 1000.0.\n";
        speed = 1000.0f;
    }
	context.simulationTimeChange = speed;
}

void SimulationManager::updateSpeed()
{
	context.simulationSpeed = context.simulationTimeChange;
}

void SimulationManager::setScene(Scene* scene) {
    scenePtr = scene;
}

bool SimulationManager::isStateOneOf(SimulationState current, std::initializer_list<SimulationState> validStates)
{
    return std::find(validStates.begin(), validStates.end(), current) != validStates.end();
}