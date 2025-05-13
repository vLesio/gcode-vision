// SimulationManager.h
#ifndef SIMULATION_MANAGER_H
#define SIMULATION_MANAGER_H

#include <queue>
#include <mutex>
#include <string>

#include "deltaTimer.h"
#include "SimulationContext.h"
#include "FilamentSimulator.h"
#include "ISimulationMode.h"
#include "scene.h"


enum class SimulationEvent {
    StepForward,
    StepBackward,
    Pause,
    Resume,
    Reset,
	Begin,
    Terminate
};

enum class SimulationState {
    Uninitialized,
    Initialized,
    Prepared,
    Running,
    Paused,
    Completed
};

class SimulationManager {
public:
    static SimulationManager& get();

    bool loadGCode(const std::string& path);
    bool initializeSimulation(
        const std::string& gcodeFile,
        float extrusionResolution,
        const std::string& printerName,
        float nozzleDiameter,
        float layerHeight,
        bool retraction,
        float bedTemp,
        float extruderTemp,
        float speed
    );

    void prepareSimulationScene(Scene* scene);
    void beginSimulation();

    void setSimulationMode(ISimulationMode* mode);
    void simulateFullPrint();

    void setScene(Scene* scene);

    void tickSimulation();
    bool tryStartSimulation(Scene* scene);

    void pauseSimulation();
    void resumeSimulation();
    void stepForward();
    void stepBackward();
    void resetSimulation();

    void enqueueEvent(SimulationEvent event);
    void processEvents();

    const SimulationContext& getContext() const;
    SimulationContext& getContextMutable();

    SimulationState getState() const;
    bool wasAlreadySimulated() const;

    // Thread safe termination handling
    std::condition_variable terminateCV;
    std::mutex terminateMutex;
    bool terminateDone = false;

private:
    SimulationManager() = default;

    SimulationContext context;
    FilamentSimulator* simulator = nullptr;
    ISimulationMode* strategy = nullptr;
    DeltaTimer timer;

    SimulationState state = SimulationState::Uninitialized;
    std::queue<SimulationEvent> eventQueue;
    std::mutex eventMutex;

    Scene* scenePtr = nullptr;

    void handleTerminate();
};

#endif