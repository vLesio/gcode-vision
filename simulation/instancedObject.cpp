#include "InstancedObject.h"

InstancedObject::InstancedObject(InstancedMesh* mesh) : mesh(mesh) {}

void InstancedObject::updateInstances() {
    mesh->updateInstances(instancePositions, instanceScales);
}

void InstancedObject::Draw(Shader& shader) {
    shader.Activate();

    mesh->DrawInstanced();
}

