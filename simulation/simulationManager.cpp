#include "SimulationManager.h"
#include <iostream>

#include "primitives.h"
#include "scene.h"

SimulationManager& SimulationManager::get() {
    static SimulationManager instance;
    return instance;
}

bool SimulationManager::loadGCode(const std::string& path) {
    context.clear();
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
    float extruderWidth,
    bool retraction,
    float bedTemp,
    float extruderTemp,
    float speed)
{
    context.clear();

    context.loadedFilename = gcodeFile;
    context.extrusionResolution = extrusionResolution;
    context.printerName = printerName;
    context.extruderWidth = extruderWidth;
    context.retractionEnabled = retraction;
    context.temperatureBed = bedTemp;
    context.temperatureExtruder = extruderTemp;
    context.userSpeed = speed;

    if (!loadGCode(gcodeFile))
        return false;

    ready = true;
    simulated = false;

    return true;
}



void SimulationManager::createSimulation(InstancedObject* filamentTarget) {
    if (simulator) {
        delete simulator;
        simulator = nullptr;
    }

    context.filamentObject = filamentTarget;
    simulator = new FilamentSimulator(filamentTarget);
    ready = true;
    simulated = false;
}

void SimulationManager::simulateFullPrint() {
    if (!ready || !simulator || context.printSteps.empty()) {
        std::cerr << "[SimulationManager] Cannot simulate. Context incomplete.\n";
        return;
    }
    simulator->simulateFullPrint(context.printSteps, context.extrusionResolution);
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
    // TODO: Implement pause logic
    std::cout << "[SimulationManager] Simulation paused (stub).\n";
}

void SimulationManager::resumeSimulation() {
    // TODO: Implement resume logic
    std::cout << "[SimulationManager] Simulation resumed (stub).\n";
}

void SimulationManager::stepForward() {
    // TODO: Implement step forward logic
    std::cout << "[SimulationManager] Simulation step forward (stub).\n";
}

void SimulationManager::stepBackward() {
    // TODO: Implement step backward logic
    std::cout << "[SimulationManager] Simulation step backward (stub).\n";
}

void SimulationManager::resetSimulation() {
    // TODO: Implement reset logic
    std::cout << "[SimulationManager] Simulation reset (stub).\n";
}
