#include "Body.h"

#include "SimulationManager.h"

Body::Body(SceneObject* bodyObject) {
	body = bodyObject;
	rail = std::make_unique<Rail>(body->children[0]);
	lastPosition = glm::vec3(0, 89.5, 0);
}

void Body::setNozzlePosition(float x, float y, float z) {

	move(glm::vec3(0, -z, 0));
	rail->setNozzlePosition(x, y, z);

}

void Body::move(glm::vec3 vec) {

	body->localTransform.resetTop();
	body->localTransform.translate(vec);
	Camera::getInstance().moveCameraWithBed(((lastPosition - vec).y)* SimulationManager::get().getContext().simulationScale);
	//std::cout << (lastPosition - vec).y << std::endl;
	lastPosition = vec;
}