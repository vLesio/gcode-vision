#include "AdaptiveScaleSimulation.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>

#include "camera.h"

// We assume that the filament will be modeled as a cuboid
float calculateFilamentWidth(float extrusionAmount, float length, float nozzleDiameter, float layerHeight) {
    float filamentVolume = glm::pi<float>() * 0.25f * nozzleDiameter * nozzleDiameter * extrusionAmount;
    return filamentVolume / (length * layerHeight);
}

// Filament as a cylinder
float calculateFilamentDiameter(float extrusionAmount, float length, float nozzleDiameter) {
	// Cylinder volume formula: V = pi * r^2 * h -> V = pi * (d/2)^2 * h
    float filamentVolume = glm::pi<float>() * 0.25f * nozzleDiameter * nozzleDiameter * extrusionAmount;

    if (length <= 0.0001f)
        return nozzleDiameter;

	// We can derive the radius from the volume and length
    float radius = std::sqrt(filamentVolume / (glm::pi<float>() * length));

	// Diameter is twice the radius
    return 2.0f * radius;
}

void AdaptiveScaleSimulation::simulate(const SimulationContext& context, FilamentSimulator& simulator)
{
    simulator.clear();

    for (const PrintStep& step : context.printSteps) {
        if (!step.extruding || step.extrusionAmount <= 0.0f)
            continue;

        simulateStep(context, simulator, step);
    }

    glm::vec3 center = simulator.computeCentroid(context.printSteps);
    center = glm::vec3(center.x, center.z, center.y);
    Camera::getInstance().setTarget(center * context.simulationScale);
}

void AdaptiveScaleSimulation::simulateStep(const SimulationContext& context, FilamentSimulator& simulator, const PrintStep& step)
{
    if (!step.extruding || step.extrusionAmount <= 0.0f)
        return;

    glm::vec3 start = glm::vec3(step.startPosition.x, step.startPosition.z, step.startPosition.y);
    glm::vec3 end = glm::vec3(step.endPosition.x, step.endPosition.z, step.endPosition.y);
    glm::vec3 dir = end - start;
    float length = glm::length(dir);
    if (length <= 0.0f)
        return;

    //float width = calculateFilamentWidth(step.extrusionAmount, length, context.nozzleDiameter, context.layerHeight);

    //glm::vec3 scale(length, width, width);
	float diameter = calculateFilamentDiameter(step.extrusionAmount, length, context.nozzleDiameter);
	glm::vec3 scale(length, diameter, diameter);
    glm::quat rotation = glm::rotation(glm::vec3(1, 0, 0), glm::normalize(dir));
    glm::vec3 position = start;

    simulator.addSegment(position * context.simulationScale, scale * context.simulationScale, rotation);
}

void AdaptiveScaleSimulation::simulatePartialStep(const SimulationContext& context, FilamentSimulator& simulator, const PrintStep& step, float progress)
{
    if (!step.extruding || step.extrusionAmount <= 0.0f)
        return;

    glm::vec3 start = glm::vec3(step.startPosition.x, step.startPosition.z, step.startPosition.y);
    glm::vec3 end = glm::vec3(step.endPosition.x, step.endPosition.z, step.endPosition.y);
    glm::vec3 dir = end - start;
    float totalLength = glm::length(dir);
    if (totalLength <= 0.0f)
        return;

    glm::vec3 partialEnd = start + dir * progress;
    float partialLength = glm::length(partialEnd - start);
    float partialExtrusion = step.extrusionAmount * progress;

    //float width = calculateFilamentWidth(partialExtrusion, partialLength, context.nozzleDiameter, context.layerHeight);

    //glm::vec3 scale(partialLength, width, width);
	float diameter = calculateFilamentDiameter(partialExtrusion, partialLength, context.nozzleDiameter);
	glm::vec3 scale(partialLength, diameter, diameter);
    glm::quat rotation = glm::rotation(glm::vec3(1, 0, 0), glm::normalize(dir));
    glm::vec3 position = start;

    simulator.setTemporarySegment(position * context.simulationScale, scale * context.simulationScale, rotation);
}

void AdaptiveScaleSimulation::simulateStepsInRange(const SimulationContext& context, FilamentSimulator& simulator, size_t count) {
    simulator.clear();

    for (size_t i = 0; i < count && i < context.printSteps.size(); ++i) {
        const PrintStep& step = context.printSteps[i];
        if (!step.extruding || step.extrusionAmount <= 0.0f)
            continue;

        simulateStep(context, simulator, step);
    }

    glm::vec3 center = simulator.computeCentroid(context.printSteps);
    center = glm::vec3(center.x, center.z, center.y);
    Camera::getInstance().setTarget(center * context.simulationScale);
}
