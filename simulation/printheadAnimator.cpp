#include "GCodeTypes.h"
#include "PrintheadAnimator.h"
#include "Primitives.h"

PrintheadAnimator::PrintheadAnimator(float scale, glm::vec3 startPosition) :simulationScale(scale), currentPosition(startPosition), startPosition(startPosition){}

PrintheadAnimator::~PrintheadAnimator() {
    if (nozzleObject && currentScene)
        currentScene->remove(nozzleObject);
}

void PrintheadAnimator::initialize() {
    nozzleObject = Primitives::createConeMarker(0.3f, 1.0f, 16, glm::vec3(0.8f, 0.8f, 0.8f));
	isInitialized = true;
    update(currentPosition);
}

void PrintheadAnimator::attachToScene(Scene* scene) {
    if (!isInitialized || !scene || !nozzleObject) return;
    currentScene = scene;
    currentScene->add(nozzleObject);
}

void PrintheadAnimator::removeFromScene(Scene* scene) {
    if (scene && nozzleObject) {
        scene->remove(nozzleObject);
    }
    currentScene = nullptr;
}

void PrintheadAnimator::update(const glm::vec3& position) {
	if (!isInitialized || !nozzleObject) return;
	glm::vec3 adjustedPosition = glm::vec3(position.x, position.z, position.y);

    currentPosition = adjustedPosition;
    nozzleObject->localTransform.resetTop();
    nozzleObject->localTransform.translate(adjustedPosition * simulationScale);
    nozzleObject->localTransform.scale(glm::vec3(1.5f) * simulationScale); 
}

void PrintheadAnimator::updateInterpolated(const glm::vec3& from, const glm::vec3& to, float progress) {
    if (!isInitialized || !nozzleObject) return;

	glm::vec3 interpolated = glm::mix(from, to, progress);
    update(interpolated);
}

void PrintheadAnimator::step(const PrintStep& step) {
	if (!isInitialized || !nozzleObject) return;
	update(step.endPosition);
}

void PrintheadAnimator::reset()
{
	if (nozzleObject) {
        update(startPosition);
	}
}
