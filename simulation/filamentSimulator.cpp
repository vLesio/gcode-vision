#include "FilamentSimulator.h"
#include "camera.h"
#include <iostream>
#include <cfloat>

FilamentSimulator::FilamentSimulator(InstancedObject* target)
    : target(target) {
}

void FilamentSimulator::addSegment(glm::vec3 position, glm::vec3 scale, glm::quat rotation) {
    instancePositions.push_back(position);
    instanceScales.push_back(scale);
    instanceRotations.push_back(rotation);
}

void FilamentSimulator::finalize() {
    target->setInstances(instancePositions, instanceScales, instanceRotations);
    target->updateInstances();
    std::cout << "[FilamentSimulator] Segments: " << instancePositions.size() << std::endl;
}

void FilamentSimulator::clear() {
    instancePositions.clear();
    instanceScales.clear();
    instanceRotations.clear();
    allSteps.clear();
}

void FilamentSimulator::simulateStepForward() {
    // Placeholder
}

void FilamentSimulator::resetSimulation() {
    clear();
    target->setInstances({}, {}, {});
    target->updateInstances();
    Camera::getInstance().reset();
    std::cout << "[FilamentSimulator] Simulation reset.\n";
}

glm::vec3 FilamentSimulator::computeCentroid(const std::vector<PrintStep>& steps) {
    glm::vec3 minPos(FLT_MAX), maxPos(-FLT_MAX);

    for (const auto& step : steps) {
        minPos = glm::min(minPos, step.startPosition);
        minPos = glm::min(minPos, step.endPosition);
        maxPos = glm::max(maxPos, step.startPosition);
        maxPos = glm::max(maxPos, step.endPosition);
    }

    return (minPos + maxPos) * 0.5f;
}
