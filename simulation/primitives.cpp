#include "Primitives.h"
#include "Mesh.h"

SceneObject* Primitives::createUnitCube() {
    Mesh* cubeMesh = Mesh::createUnitCube();
    return new SceneObject(cubeMesh);
}