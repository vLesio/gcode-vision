#ifndef SIMULATION_MANAGER_H
#define SIMULATION_MANAGER_H

#include "deltaTimer.h"
#include "SimulationContext.h"
#include "FilamentSimulator.h"
#include "ISimulationMode.h"

class SimulationManager {
public:
    static SimulationManager& get();

    bool loadGCode(const std::string& path);
    void createSimulation();
    bool initializeSimulation(const std::string& gcodeFile, float extrusionResolution, const std::string& printerName, float nozzleDiameter, float layerHeight, bool retraction, float bedTemp, float extruderTemp, float speed);

    void simulateFullPrint();

    void setSimulationMode(ISimulationMode* mode);

    const SimulationContext& getContext() const;
    SimulationContext& getContextMutable();

    bool isReady() const;
    void markSimulated();
    bool wasAlreadySimulated() const;

    void pauseSimulation();
    void resumeSimulation();
    void stepForward();
    void stepBackward();
    void recomputeStepsUpTo(size_t index);
    void resetSimulation();
    void startSimulation();
    void tickSimulation();

private:
    SimulationManager() = default;

    SimulationContext context;
    FilamentSimulator* simulator = nullptr;
    ISimulationMode* strategy = nullptr;

	DeltaTimer timer;

    bool ready = false;
    bool simulated = false;
};

#endif
