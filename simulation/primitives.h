#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "SceneObject.h"

class Primitives {
public:
    static SceneObject* createUnitCube();
    static InstancedObject* createInstancedCube();
	static InstancedObject* createDirectionalCube();
    static SceneObject* createPlane(float size);
    static InstancedObject* createDirectedCylinder(int segments, float radius, float length);
    static SceneObject* createConeMarker(float radius, float height, int segments);
};

#endif
