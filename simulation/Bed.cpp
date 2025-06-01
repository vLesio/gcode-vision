#include "Bed.h"

Bed::Bed(SceneObject* bedObject) {
	bed = bedObject;
	body = std::make_unique<Body>(bed->children[0]);
}

void Bed::setNozzlePosition(float x, float y, float z) {
	body->setNozzlePosition(x, y, z);
}