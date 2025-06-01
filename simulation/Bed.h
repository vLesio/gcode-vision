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
	void SetNozzlePosition(float x, float y, float z);
	SceneObject* GetBed() const { return bed; }
};
#endif