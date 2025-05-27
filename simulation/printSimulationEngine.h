#ifndef PRINT_SIMULATION_ENGINE_H
#define PRINT_SIMULATION_ENGINE_H

#include "FilamentSimulator.h"
#include "SimulationContext.h"


class PrintSimulationEngine {
public:
    PrintSimulationEngine();
    void attachObjects(InstancedObject* filament, InstancedObject* tempSegment);
    void simulate(const SimulationContext& context);
    void simulateStep(const SimulationContext& context, const PrintStep& step);
    void simulatePartialStep(const SimulationContext& context, const PrintStep& step, float progress);
    void simulateStepsInRange(const SimulationContext& context, size_t count);
    void reset();

    static glm::vec3 computeCentroid(const std::vector<PrintStep>& steps);
private:
    FilamentSimulator filamentSimulator;
};
#endif