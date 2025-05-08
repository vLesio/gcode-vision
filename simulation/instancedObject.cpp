#include "instancedObject.h"

InstancedObject::InstancedObject(InstancedMesh* mesh) : mesh(mesh) {}

void InstancedObject::setInstances(const std::vector<glm::vec3>& positions, const std::vector<float>& scales) {
    instancePositions = positions;
    instanceScales = scales;
}

void InstancedObject::updateInstances() {
    if (mesh && instancePositions.size() == instanceScales.size()) {
        mesh->updateInstances(instancePositions, instanceScales);
    }
    else {
        std::cerr << "[InstancedObject] Warning: Mismatched instance data.\n";
    }
}

void InstancedObject::Draw(Shader& shader) {
    shader.Activate();
    mesh->DrawInstanced();
}
