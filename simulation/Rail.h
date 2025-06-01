#ifndef RAIL_H
#define RAIL_H

#include "sceneObject.h"
#include "Nozzle.h"

class Rail
{
private:
	std::unique_ptr<Nozzle> nozzle;
	SceneObject* rail;
public:
	Rail(SceneObject*);
	void setNozzlePosition(float x, float y, float z);
	void move(glm::vec3);


};
#endif