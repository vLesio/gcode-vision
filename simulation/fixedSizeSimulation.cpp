#include "camera.h"
#include "FixedSizeSimulation.h"
#include <glm/glm.hpp>
#include <iostream>

void FixedSizeSimulation::simulate(const SimulationContext& context, FilamentSimulator& simulator) {
    const auto& steps = context.printSteps;
    float resolution = context.extrusionResolution;

    simulator.clear();

    for (const PrintStep& step : steps) {
        if (!step.extruding || step.extrusionAmount <= 0.0f)
            continue;

        glm::vec3 start = glm::vec3(step.startPosition.x, step.startPosition.z, step.startPosition.y);
        glm::vec3 end = glm::vec3(step.endPosition.x, step.endPosition.z, step.endPosition.y);


        glm::vec3 dir = end - start;
        float length = glm::length(dir);
        if (length <= 0.0f)
            continue;

        int segments = std::max(1, static_cast<int>(length / resolution));
        glm::vec3 unit = dir / static_cast<float>(segments);

        for (int i = 0; i < segments; ++i) {
            glm::vec3 pos = start + unit * static_cast<float>(i);
            glm::quat rot = glm::quat();
            glm::vec3 scale(resolution);

            simulator.addSegment(pos, scale, rot);
        }
    }

    glm::vec3 center = simulator.computeCentroid(steps);
    center = glm::vec3(center.x, center.z, center.y);
    Camera::getInstance().setTarget(center);
}

void FixedSizeSimulation::simulateStep(const SimulationContext& context, FilamentSimulator& simulator, const PrintStep& step) {
    std::cout << "[FixedSizeSimulation] simulateStep not implemented yet.\n";
}

void FixedSizeSimulation::simulatePartialStep(const SimulationContext& context, FilamentSimulator& simulator, const PrintStep& step, float progress) {
    std::cout << "[FixedSizeSimulation] simulatePartialStep not implemented yet.\n";
}

void FixedSizeSimulation::simulateStepsInRange(const SimulationContext& context, FilamentSimulator& simulator, size_t count) {
	std::cout << "[FixedSizeSimulation] simulateStepsInRange not implemented yet.\n";
}