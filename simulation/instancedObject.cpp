#include "InstancedObject.h"
#include <iostream>

InstancedObject::InstancedObject(std::unique_ptr<InstancedMesh> mesh)
    : mesh(std::move(mesh)) {
}

void InstancedObject::Draw(Shader& shader) {
    shader.Activate();
    mesh->DrawInstanced();
}

void InstancedObject::setStaticInstances(const std::vector<glm::vec3>& positions,
    const std::vector<glm::vec3>& scales,
    const std::vector<glm::quat>& rotations) {
    instancePositions = positions;
    instanceScales = scales;
    instanceRotations = rotations;
}

void InstancedObject::uploadStaticToGPU() {
    if (!mesh) return;

    if (instancePositions.size() == instanceScales.size() &&
        instanceScales.size() == instanceRotations.size()) {
        mesh->uploadStaticInstances(instancePositions, instanceScales, instanceRotations);
    }
    else {
        std::cerr << "[InstancedObject] Mismatched instance data.\n";
    }
}

void InstancedObject::appendDynamicInstance(const glm::vec3& pos, const glm::vec3& scale, const glm::quat& rot) {
    instancePositions.push_back(pos);
    instanceScales.push_back(scale);
    instanceRotations.push_back(rot);

    if (mesh)
        mesh->appendInstance(pos, scale, rot);
}

void InstancedObject::updateDynamicInstance(size_t i, const glm::vec3& pos, const glm::vec3& scale, const glm::quat& rot) {
    if (i >= instancePositions.size()) {
        std::cerr << "[InstancedObject] Instance index out of bounds!\n";
        return;
    }

    instancePositions[i] = pos;
    instanceScales[i] = scale;
    instanceRotations[i] = rot;

    if (mesh)
        mesh->uploadInstance(i, pos, scale, rot);
}

void InstancedObject::clear() {
    instancePositions.clear();
    instanceScales.clear();
    instanceRotations.clear();

    if (!mesh) return;

    if (mesh->getInstanceMode() == InstanceBufferMode::Dynamic) {
        mesh->resetDynamicBuffer();
    }
}

void InstancedObject::reserveInstances(size_t maxCount) {
    if (mesh)
        mesh->initializeDynamicBuffer(maxCount);
}