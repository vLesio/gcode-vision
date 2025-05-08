#include "filamentSimulator.h"

#include <glm/ext/scalar_constants.hpp>

#include "camera.h"

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

		// Swap Y and Z for the simulation
        glm::vec3 start = glm::vec3(step.startPosition.x, step.startPosition.z, step.startPosition.y);
        glm::vec3 end = glm::vec3(step.endPosition.x, step.endPosition.z, step.endPosition.y);

		start *= 0.01f; // Scale down for simulation
		end *= 0.01f; // Scale down for simulation
		resolution = 0.001f; // Scale down for simulation

        glm::vec3 dir = end - start;
        float length = glm::length(dir);
        if (length <= 0.0f)
            continue;

        glm::vec3 norm = glm::normalize(dir);
        int segments = std::max(1, static_cast<int>(length / resolution));

        for (int i = 0; i < segments; ++i) {
            float t = static_cast<float>(i) / segments;
            glm::vec3 pos = start + t * dir;
            instancePositions.push_back(pos);
            instanceScales.push_back(resolution * 0.5f); // scale/radius
        }

        glm::vec3 center = computeCentroid(allSteps);
        center = glm::vec3(center.x, center.z, center.y) * 0.01f; // Swap Y-Z + scale like filament
        Camera::getInstance().setTarget(center);
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

glm::vec3 FilamentSimulator::computeCentroid(const std::vector<PrintStep>& steps) {
    glm::vec3 minPos(FLT_MAX), maxPos(-FLT_MAX);

    for (const auto& step : steps) {
        glm::vec3 p1 = step.startPosition;
        glm::vec3 p2 = step.endPosition;

        minPos = glm::min(minPos, p1);
        minPos = glm::min(minPos, p2);
        maxPos = glm::max(maxPos, p1);
        maxPos = glm::max(maxPos, p2);
    }

    return (minPos + maxPos) * 0.5f;
}
