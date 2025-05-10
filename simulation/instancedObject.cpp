#include "instancedObject.h"

InstancedObject::InstancedObject(InstancedMesh* mesh) : mesh(mesh) {}

void InstancedObject::setInstances(const std::vector<glm::vec3>& positions,
    const std::vector<glm::vec3>& scales,
    const std::vector<glm::quat>& rotations) {
    instancePositions = positions;
    instanceScales = scales;
    instanceRotations = rotations;
}

void InstancedObject::updateInstances() {
    if (mesh && instancePositions.size() == instanceScales.size() && instanceScales.size() == instanceRotations.size()) {
        mesh->updateInstances(instancePositions, instanceScales, instanceRotations);
    }
    else {
        std::cerr << "[InstancedObject] Mismatched instance data.\n";
    }
}


void InstancedObject::Draw(Shader& shader) {
    shader.Activate();
    mesh->DrawInstanced();
}
