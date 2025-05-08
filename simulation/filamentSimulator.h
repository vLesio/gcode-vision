#ifndef FILAMENT_SIMULATOR_H
#define FILAMENT_SIMULATOR_H

#include <vector>
#include <glm/glm.hpp>

#include "GCodeTypes.h"
#include "instancedObject.h"

class FilamentSimulator {
public:
    FilamentSimulator(InstancedObject* target);

    void simulateFullPrint(const std::vector<PrintStep>& steps, float resolution = 0.2f);

    void simulateStepForward();
    void resetSimulation();
    glm::vec3 computeCentroid(const std::vector<PrintStep>& steps);

private:
    InstancedObject* target;
    std::vector<PrintStep> allSteps;
    float resolution;

    size_t currentStep = 0;
};

#endif
