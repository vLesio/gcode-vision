#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#include "InstancedMesh.h"
#include "SceneObject.h"

class Primitives {
public:
    static SceneObject* createUnitCube();
    static InstancedObject* createInstancedCube();
    static SceneObject* createTexturedPlane(float size);
};

#endif
