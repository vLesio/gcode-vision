#include "SimulationManager.h"
#include <iostream>
#include <condition_variable>
#include "primitives.h"
#include "scene.h"

/// Simulation lifecycle management

bool SimulationManager::initializeSimulation(
    const std::string& gcodeFile,
    float extrusionResolution,
    const std::string& printerName,
    float nozzleDiameter,
    float layerHeight,
    bool retraction,
    float bedTemp,
    float extruderTemp,
    float speed)
{
    context.clear();
    context.loadedFilename = gcodeFile;
    context.extrusionResolution = extrusionResolution;
    context.printerName = printerName;
    context.nozzleDiameter = nozzleDiameter;
    context.layerHeight = layerHeight;
    context.retractionEnabled = retraction;
    context.temperatureBed = bedTemp;
    context.temperatureExtruder = extruderTemp;
    context.simulationSpeed = speed;

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

    if (simulator) {
        delete simulator;
        simulator = nullptr;
    }

    if (!strategy) {
        std::cerr << "[SimulationManager] No simulation strategy set.\n";
        return;
    }

    std::string mode = strategy->getName();
    context.filamentObject = (mode == "adaptive") ?
        Primitives::createDirectionalCube() :
        Primitives::createInstancedCube();

    scene->addInstanced(context.filamentObject);
    simulator = new FilamentSimulator(context.filamentObject);
    state = SimulationState::Prepared;
}

void SimulationManager::beginSimulation() {
    if (state != SimulationState::Prepared) {
        std::cerr << "[SimulationManager] Cannot begin. Scene not prepared.\n";
        return;
    }

    simulator->clear();
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
    if (state != SimulationState::Paused) {
        std::cerr << "[SimulationManager] Cannot step forward unless paused.\n";
        return;
    }

    if (!context.filamentObject || !context.filamentObject->getMesh()) {
        std::cerr << "[SimulationManager] Step failed: mesh is null.\n";
        return;
    }

    if (context.currentStepIndex >= context.printSteps.size()) return;
    const PrintStep& step = context.printSteps[context.currentStepIndex];
    strategy->simulateStep(context, *simulator, step);
    simulator->updateBuffers();
    context.currentStepIndex++;
    context.simulationTime = 0.0f;

    std::cout << "[SimulationManager] Step forward: " << context.currentStepIndex << "\n";
}

void SimulationManager::stepBackward() {
    if (state != SimulationState::Paused) {
        std::cerr << "[SimulationManager] Cannot step backward unless paused.\n";
        return;
    }

    if (context.currentStepIndex > 0) {
        context.currentStepIndex--;
        recomputeStepsUpTo(context.currentStepIndex);
        context.simulationTime = 0.0f;
        std::cout << "[SimulationManager] Step backward: " << context.currentStepIndex << "\n";
    }
}

void SimulationManager::recomputeStepsUpTo(size_t index) {
    if (strategy && simulator) {
        simulator->clear();
        for (size_t i = 0; i < index; ++i) {
            strategy->simulateStep(context, *simulator, context.printSteps[i]);
        }
        simulator->updateBuffers();
    }
}

void SimulationManager::resetSimulation() {
    if (simulator) {
        simulator->resetSimulation();
    }
    context.resetRuntime();
    timer.pause();
    state = SimulationState::Prepared;
    std::cout << "[SimulationManager] Simulation reset.\n";
}

void SimulationManager::tickSimulation() {
    if (state != SimulationState::Running || !simulator)
        return;

    float deltaTime = timer.tick();
    if (deltaTime <= 0.0f)
        return;

    context.simulationTime += deltaTime * context.simulationSpeed;

    while (context.currentStepIndex < context.printSteps.size()) {
        const PrintStep& step = context.printSteps[context.currentStepIndex];
        float moveLength = glm::distance(step.startPosition, step.endPosition);
        float speed = (step.speed > 0.0f) ? step.speed : 1500.0f;
        float moveDuration = moveLength / (speed / 60.0f);

        if (context.simulationTime < moveDuration)
            break;

        strategy->simulateStep(context, *simulator, step);
        simulator->updateBuffers();

        context.simulationTime -= moveDuration;
        context.currentStepIndex++;
    }

    if (context.currentStepIndex >= context.printSteps.size() && state == SimulationState::Running) {
        state = SimulationState::Completed;
        std::cout << "[SimulationManager] Simulation completed.\n";
    }
}

void SimulationManager::handleTerminate() {
    {
        std::lock_guard<std::mutex> lock(terminateMutex);
        terminateDone = false;
    }

    if (simulator) {
        simulator->resetSimulation();
        delete simulator;
        simulator = nullptr;
    }

    strategy = nullptr;

    if (context.filamentObject && scenePtr) {
        scenePtr->removeInstanced(context.filamentObject);
    }

    context.filamentObject = nullptr;
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

/// REST API Event queue management

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
        }
    }
}

/// Other methods

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
    return true;
}

void SimulationManager::setSimulationMode(ISimulationMode* mode) {
    strategy = mode;
}

void SimulationManager::simulateFullPrint() {
    if (state != SimulationState::Prepared || !strategy || !simulator) {
        std::cerr << "[SimulationManager] Cannot simulate full print.\n";
        return;
    }

    strategy->simulate(context, *simulator);
    simulator->updateBuffers();
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

void SimulationManager::setScene(Scene* scene) {
    scenePtr = scene;
}
