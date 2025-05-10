#include "SimulationManager.h"
#include <iostream>

#include "primitives.h"
#include "scene.h"

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

    ready = true;
    simulated = false;

    return true;
}

void SimulationManager::createSimulation() {
    if (simulator) {
        delete simulator;
        simulator = nullptr;
    }

    if (!strategy) {
        std::cerr << "[SimulationManager] No simulation strategy set. Cannot create simulation.\n";
        return;
    }

    InstancedObject* filamentTarget = nullptr;
    std::string mode = strategy->getName();

    if (mode == "adaptive") {
        filamentTarget = Primitives::createDirectionalCube();
    }
    else {
        filamentTarget = Primitives::createInstancedCube(); 
    }

    context.filamentObject = filamentTarget;
    simulator = new FilamentSimulator(filamentTarget);
    ready = true;
    simulated = false;
}



void SimulationManager::setSimulationMode(ISimulationMode* mode) {
    strategy = mode;
}

void SimulationManager::simulateFullPrint() {
    if (!ready || !simulator || context.printSteps.empty()) {
        std::cerr << "[SimulationManager] Cannot simulate. Context incomplete.\n";
        return;
    }

    if (!strategy) {
        std::cerr << "[SimulationManager] No simulation strategy set.\n";
        return;
    }

    strategy->simulate(context, *simulator);
    simulated = true;
}

const SimulationContext& SimulationManager::getContext() const {
    return context;
}

SimulationContext& SimulationManager::getContextMutable() {
    return context;
}

bool SimulationManager::isReady() const {
    return ready;
}

void SimulationManager::markSimulated() {
    simulated = true;
}

bool SimulationManager::wasAlreadySimulated() const {
    return simulated;
}

void SimulationManager::pauseSimulation() {
    context.isPaused = true;
    timer.pause();
}

void SimulationManager::resumeSimulation() {
    context.isPaused = false;
    timer.resume();
}

void SimulationManager::stepForward() {
    if (!context.isPaused) return;

    if (context.currentStepIndex >= context.printSteps.size()) return;
    const PrintStep& step = context.printSteps[context.currentStepIndex];
    strategy->simulateStep(context, *simulator, step);
	simulator->updateBuffers();
    context.currentStepIndex++;
    context.simulationTime = 0.0f;
	std::cout << "[SimulationManager] Step forward: " << context.currentStepIndex << "\n";
}



void SimulationManager::stepBackward() {
    if (!context.isPaused) return;

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
    simulated = false;
    std::cout << "[SimulationManager] Simulation reset.\n";
}

void SimulationManager::startSimulation() {
    if (!ready || !simulator || context.printSteps.empty() || !strategy) {
        std::cerr << "[SimulationManager] Cannot start simulation.\n";
        return;
    }

    simulator->clear(); 
    context.simulationTime = 0.0f;
    context.currentStepIndex = 0;
    context.isPaused = false;

    timer.reset();

	std::cout << "[SimulationManager] Simulation started.\n";
}

void SimulationManager::tickSimulation() {
    if (!ready || !simulator || context.isPaused)
        return;

    float deltaTime = timer.tick();
    if (deltaTime <= 0.0f)
        return;

    context.simulationTime += deltaTime * context.simulationSpeed;

    while (context.currentStepIndex < context.printSteps.size()) {
        const PrintStep& step = context.printSteps[context.currentStepIndex];

        float moveLength = glm::distance(step.startPosition, step.endPosition);
        float speed = (step.speed > 0.0f) ? step.speed : 1500.0f;
        if (step.speed <= 0.0f)
        {
			//std::cerr << "[SimulationManager] Warning: Step " << context.currentStepIndex << " has no speed set. Using default speed of 1500 mm/min.\n";
        }
        float moveDuration = moveLength / (speed / 60.0f);

        if (context.simulationTime < moveDuration)
            break;

        strategy->simulateStep(context, *simulator, step);
        simulator->updateBuffers();

        context.simulationTime -= moveDuration;
        context.currentStepIndex++;
    }

    if (context.currentStepIndex >= context.printSteps.size() && !simulated) {
        simulated = true;
		std::cout << "[SimulationManager] Simulation completed.\n";
    }
}
