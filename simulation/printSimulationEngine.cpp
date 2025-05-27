#include "printSimulationEngine.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>

#include "camera.h"

PrintSimulationEngine::PrintSimulationEngine(): filamentSimulator(nullptr){}

static float calculateFilamentDiameter(float extrusionAmount, float length, float nozzleDiameter) {
    float filamentVolume = glm::pi<float>() * 0.25f * nozzleDiameter * nozzleDiameter * extrusionAmount;
    if (length <= 0.0001f) return nozzleDiameter;
    float radius = std::sqrt(filamentVolume / (glm::pi<float>() * length));
    return 2.0f * radius;
}

void PrintSimulationEngine::attachObjects(InstancedObject* filament, InstancedObject* tempSegment) {
    filamentSimulator = FilamentSimulator(filament);
    filamentSimulator.attachTemporaryObject(tempSegment);
}

void PrintSimulationEngine::simulate(const SimulationContext& context) {
    filamentSimulator.clear();

    for (const auto& step : context.printSteps) {
        if (step.extruding && step.extrusionAmount > 0.0f) {
            simulateStep(context, step);
        }
    }

    glm::vec3 center = computeCentroid(context.printSteps);
    Camera::getInstance().setTarget(glm::vec3(center.x, center.z, center.y) * context.simulationScale);
}

void PrintSimulationEngine::simulateStep(const SimulationContext& context, const PrintStep& step) {
    filamentSimulator.clearTemporarySegment();
    glm::vec3 start = glm::vec3(step.startPosition.x, step.startPosition.z, step.startPosition.y);
    glm::vec3 end = glm::vec3(step.endPosition.x, step.endPosition.z, step.endPosition.y);
    glm::vec3 dir = end - start;

    float length = glm::length(dir);
    if (length <= 0.0f) return;

    float diameter = calculateFilamentDiameter(step.extrusionAmount, length, context.nozzleDiameter);
    glm::vec3 scale(length, diameter, diameter);
    glm::quat rotation = glm::rotation(glm::vec3(1, 0, 0), glm::normalize(dir));

    filamentSimulator.addSegment(start * context.simulationScale, scale * context.simulationScale, rotation);
}

void PrintSimulationEngine::simulatePartialStep(const SimulationContext& context, const PrintStep& step, float progress) {
    glm::vec3 start = glm::vec3(step.startPosition.x, step.startPosition.z, step.startPosition.y);
    glm::vec3 end = glm::vec3(step.endPosition.x, step.endPosition.z, step.endPosition.y);
    glm::vec3 dir = end - start;

    float totalLength = glm::length(dir);
    if (totalLength <= 0.0f) return;

    glm::vec3 partialEnd = start + dir * progress;
    float partialLength = glm::length(partialEnd - start);
    float partialExtrusion = step.extrusionAmount * progress;

    float diameter = calculateFilamentDiameter(partialExtrusion, partialLength, context.nozzleDiameter);
    glm::vec3 scale(partialLength, diameter, diameter);
    glm::quat rotation = glm::rotation(glm::vec3(1, 0, 0), glm::normalize(dir));

    filamentSimulator.setTemporarySegment(start * context.simulationScale, scale * context.simulationScale, rotation);
}

void PrintSimulationEngine::simulateStepsInRange(const SimulationContext& context, size_t count) {
    filamentSimulator.clear();

    for (size_t i = 0; i < count && i < context.printSteps.size(); ++i) {
        const PrintStep& step = context.printSteps[i];
        if (step.extruding && step.extrusionAmount > 0.0f)
            simulateStep(context, step);
    }
}

void PrintSimulationEngine::reset() {
    filamentSimulator.clear();
	std::cout << "[PrintSimulationEngine] Simulation reset.\n";
}

glm::vec3 PrintSimulationEngine::computeCentroid(const std::vector<PrintStep>& steps) {
    glm::vec3 minPos(FLT_MAX), maxPos(-FLT_MAX);

    for (const auto& step : steps) {
        minPos = glm::min(minPos, step.startPosition);
        minPos = glm::min(minPos, step.endPosition);
        maxPos = glm::max(maxPos, step.startPosition);
        maxPos = glm::max(maxPos, step.endPosition);
    }

    return (minPos + maxPos) * 0.5f;
}
