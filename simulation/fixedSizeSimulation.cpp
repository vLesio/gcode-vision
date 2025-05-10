#include "camera.h"
#include "FixedSizeSimulation.h"


void FixedSizeSimulation::simulate(const SimulationContext& context, FilamentSimulator& simulator) {
    const auto& steps = context.printSteps;
	float resolution = context.extrusionResolution;

    simulator.clear();

    for (const PrintStep& step : steps) {
        if (!step.extruding || step.extrusionAmount <= 0.0f)
            continue;

        glm::vec3 start = glm::vec3(step.startPosition.x, step.startPosition.z, step.startPosition.y);
        glm::vec3 end = glm::vec3(step.endPosition.x, step.endPosition.z, step.endPosition.y);

        start *= simulator.simulationScale;
        end *= simulator.simulationScale;

        glm::vec3 dir = end - start;
        float length = glm::length(dir);
        if (length <= 0.0f)
            continue;

        int segments = std::max(1, static_cast<int>(length / resolution));
        for (int i = 0; i < segments; ++i) {
            float t = static_cast<float>(i) / segments;
            glm::vec3 pos = start + t * dir;
            simulator.addSegment(pos, glm::vec3(resolution), glm::quat()); 
        }
    }

    glm::vec3 center = simulator.computeCentroid(steps);
    center = glm::vec3(center.x, center.z, center.y) * simulator.simulationScale;
    Camera::getInstance().setTarget(center);

    simulator.finalize();
}

void FixedSizeSimulation::simulateStep(const SimulationContext& context, FilamentSimulator& sim, const PrintStep& step) {
	std::cout << "[FixedSizeSimulation] No step simulation for fixed size yet TODO\n";
}
