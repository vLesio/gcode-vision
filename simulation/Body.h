#ifndef BODY_H
#define BODY_H

#include "sceneObject.h"
#include "Rail.h"
#include "camera.h"

class Body
{
private:
	std::unique_ptr<Rail> rail;
	SceneObject* body;
	glm::vec3 lastPosition;
public:
	Body(SceneObject*);
	void setNozzlePosition(float x, float y, float z);
	void move(glm::vec3);
};
#endif