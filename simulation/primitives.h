#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "SceneObject.h"

class Primitives {
public:
    static SceneObject* createUnitCube();
    static InstancedObject* createInstancedCube();
	static InstancedObject* createDirectionalCube();
    static SceneObject* createTexturedPlane(float size);
    static InstancedObject* createDirectedCylinder(int segments, float radius, float length);
};

#endif
