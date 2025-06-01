#include "Rail.h"

Rail::Rail(SceneObject* railObject)
{
	rail = railObject;
	rail->children[1]->setParent(rail->children[0]);
	nozzle = std::make_unique<Nozzle>(rail->children[0]);
}

void Rail::SetNozzlePosition(float x, float y, float z) {
	Move(glm::vec3(0, 0, y));
	nozzle->SetNozzlePosition(x, y, z);
}

void Rail::Move(glm::vec3 offset) {
	rail->localTransform.resetTop();
	rail->localTransform.translate(offset);
}