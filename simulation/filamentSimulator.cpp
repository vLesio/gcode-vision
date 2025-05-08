#include "filamentSimulator.h"

FilamentSimulator::FilamentSimulator(InstancedObject* target)
    : target(target), resolution(0.2f), currentStep(0) {
}

void FilamentSimulator::simulateFullPrint(const std::vector<PrintStep>& steps, float resolution) {
    allSteps = steps;
    this->resolution = resolution;
    currentStep = steps.size();

    std::vector<glm::vec3> instancePositions;
    std::vector<float> instanceScales;

    for (const PrintStep& step : steps) {
        if (!step.extruding || step.extrusionAmount <= 0.0f)
            continue;

        glm::vec3 dir = step.endPosition - step.startPosition;
        float length = glm::length(dir);
        if (length <= 0.0f)
            continue;

        glm::vec3 norm = glm::normalize(dir);
        int segments = std::max(1, static_cast<int>(length / resolution));

        for (int i = 0; i < segments; ++i) {
            float t = static_cast<float>(i) / segments;
            glm::vec3 pos = step.startPosition + t * dir;
            instancePositions.push_back(pos);
            instanceScales.push_back(resolution * 0.5f); // radius/scale
        }
    }

    target->setInstances(instancePositions, instanceScales);
    target->updateInstances();
}

// MOCKS

void FilamentSimulator::simulateStepForward() {
    // Placeholder
}

void FilamentSimulator::resetSimulation() {
    currentStep = 0;
    target->setInstances({}, {});
    target->updateInstances();
}
