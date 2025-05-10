#include "AdaptiveScaleSimulation.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>

#include "camera.h"

void AdaptiveScaleSimulation::simulate(const std::vector<PrintStep>& steps, FilamentSimulator& simulator) {
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

        glm::vec3 scale(length * simulator.simulationScale, 0.002f, 0.002f);
        glm::quat rotation = glm::rotation(glm::vec3(1, 0, 0), glm::normalize(dir));
        glm::vec3 position = start * simulator.simulationScale;

        simulator.addSegment(position, scale, rotation);
    }

    glm::vec3 center = simulator.computeCentroid(steps);
    center = glm::vec3(center.x, center.z, center.y) * simulator.simulationScale;
    Camera::getInstance().setTarget(center);

    simulator.finalize();
}
