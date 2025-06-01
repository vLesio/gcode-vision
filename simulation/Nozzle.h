#ifndef NOZZLE_H
#define NOZZLE_H

#include "sceneObject.h"

class Nozzle
{
private:
	SceneObject* nozzle;
public:
	Nozzle(SceneObject*);
	void SetNozzlePosition(float x, float y, float z);
	void Move(glm::vec3);
};
#endif