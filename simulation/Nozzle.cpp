#include "Nozzle.h"

Nozzle::Nozzle(SceneObject* nozzleObject) {
	nozzle = nozzleObject;
	nozzle->children[0]->localTransform.resetTop();
	nozzle->children[0]->localTransform.translate(glm::vec3(0, 0, -10));
}

void Nozzle::SetNozzlePosition(float x, float y, float z) {
	Move(glm::vec3(x, 0, 0));
}

void Nozzle::Move(glm::vec3 vec) {
	nozzle->localTransform.resetTop();
	nozzle->localTransform.translate(vec);
}