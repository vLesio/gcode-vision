#ifndef SIMULATION_MANAGER_H
#define SIMULATION_MANAGER_H

#include "SimulationContext.h"
#include "FilamentSimulator.h"

class SimulationManager {
public:
    static SimulationManager& get();

    bool loadGCode(const std::string& path);
    void createSimulation(InstancedObject* filamentTarget);
    bool initializeSimulation(const std::string& gcodeFile, float extrusionResolution, const std::string& printerName, float extruderWidth, bool retraction, float bedTemp, float extruderTemp, float speed);
    void simulateFullPrint();

    const SimulationContext& getContext() const;
    SimulationContext& getContextMutable();

    bool isReady() const;
    void markSimulated();
    bool wasAlreadySimulated() const;

    // For future control over simulation steps
    void pauseSimulation();
    void resumeSimulation();
    void stepForward();
    void stepBackward();
    void resetSimulation();

private:
    SimulationManager() = default;

    SimulationContext context;
    FilamentSimulator* simulator = nullptr;
    bool ready = false;
    bool simulated = false;
};

#endif