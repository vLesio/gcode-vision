#ifndef NOZZLE_H
#define NOZZLE_H

#include "sceneObject.h"

class Nozzle
{
private:
	SceneObject* nozzle;
public:
	Nozzle(SceneObject*);
	void setNozzlePosition(float x, float y, float z);
	void move(glm::vec3);
};
#endif