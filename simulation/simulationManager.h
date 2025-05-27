// SimulationManager.h
#ifndef SIMULATION_MANAGER_H
#define SIMULATION_MANAGER_H

#include <queue>
#include <mutex>
#include <string>

#include "deltaTimer.h"
#include "SimulationContext.h"
#include "PrintheadAnimator.h"
#include "PrintSimulationEngine.h"
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
    DeltaTimer timer;

	std::unique_ptr<PrintSimulationEngine> printEngine = nullptr;
	std::unique_ptr<PrintheadAnimator> printheadAnimator = nullptr;

    SimulationState state = SimulationState::Uninitialized;
    std::queue<SimulationEvent> eventQueue;
    std::mutex eventMutex;

    Scene* scenePtr = nullptr;

    void handleTerminate();
};

#endif