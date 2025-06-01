#include "Body.h"

Body::Body(SceneObject* bodyObject) {
	body = bodyObject;
	rail = std::make_unique<Rail>(body->children[0]);
	lastPosition = glm::vec3(0, 89.5, 0);
}

void Body::SetNozzlePosition(float x, float y, float z) {

	Move(glm::vec3(0, -z, 0));
	rail->SetNozzlePosition(x, y, z);

}

void Body::Move(glm::vec3 vec) {

	body->localTransform.resetTop();
	body->localTransform.translate(vec);
	Camera::getInstance().moveCameraWithBed(((lastPosition - vec).y)/100);
	//std::cout << (lastPosition - vec).y << std::endl;
	lastPosition = vec;
}