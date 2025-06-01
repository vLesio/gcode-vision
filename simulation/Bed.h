#ifndef BED_H
#define BED_H

#include "sceneObject.h"
#include "Body.h"

class Bed
{
private:
	std::unique_ptr<Body> body;
	SceneObject* bed;

public:
	Bed(SceneObject*);
	void setNozzlePosition(float x, float y, float z);
	SceneObject* getBed() const { return bed; }
};
#endif