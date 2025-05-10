#ifndef FILAMENT_SIMULATOR_H
#define FILAMENT_SIMULATOR_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "GCodeTypes.h"
#include "instancedObject.h"

class FilamentSimulator {
public:
    explicit FilamentSimulator(InstancedObject* target);

    void addSegment(glm::vec3 position, glm::vec3 scale, glm::quat rotation);
    void finalize();
    void clear();

    void simulateStepForward(); // TODO
    void resetSimulation();

    glm::vec3 computeCentroid(const std::vector<PrintStep>& steps);

    float simulationScale = 0.01f;

private:
    InstancedObject* target;

    std::vector<glm::vec3> instancePositions;
    std::vector<glm::vec3> instanceScales;
    std::vector<glm::quat> instanceRotations;

    std::vector<PrintStep> allSteps;
    size_t currentStep = 0;
};

#endif
