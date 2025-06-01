#include "GCodeTypes.h"
#include "materialManager.h"
#include "PrintheadAnimator.h"
#include "Primitives.h"

PrintheadAnimator::PrintheadAnimator(float scale, glm::vec3 startPosition) :simulationScale(scale), currentPosition(startPosition), startPosition(startPosition){}

PrintheadAnimator::~PrintheadAnimator() {
    if (nozzleObject && currentScene)
        currentScene->remove(nozzleObject);
}

void PrintheadAnimator::initialize() {
    nozzleObject = Primitives::createConeMarker(0.3f, 1.0f, 16);
	if (MaterialManager::get("cone") == nullptr) {
		MaterialManager::create("cone", glm::vec3(0.1, 0.6, 0.8), 1.0f);
		auto  mat= MaterialManager::get("cone");
        mat->specularStrength = 0.05f;
	}
	nozzleObject->setMaterial(MaterialManager::get("cone"));

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
	glm::vec3 actualGlobalPosition = adjustedPosition * simulationScale;
    nozzleObject->localTransform.translate(actualGlobalPosition);
    nozzleObject->localTransform.scale(glm::vec3(1.5f) * simulationScale); 
	printer->setNozzlePosition(adjustedPosition.x, adjustedPosition.y, adjustedPosition.z);
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

void PrintheadAnimator::attachPrinter(std::shared_ptr<Printer> printer)
{
    this->printer = printer;
}
