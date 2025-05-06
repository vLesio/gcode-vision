#include "InstancedObject.h"

InstancedObject::InstancedObject(InstancedMesh* mesh) : mesh(mesh) {}

void InstancedObject::updateInstances() {
    mesh->updateInstances(instancePositions, instanceScales);
}

void InstancedObject::Draw(Shader& shader) {
    shader.Activate();
    glUniform1i(glGetUniformLocation(shader.ID, "isInstanced"), true);
    glUniform1f(glGetUniformLocation(shader.ID, "useTexture"), hasTexture ? 1.0f : 0.0f);

    if (hasTexture && texture) {
        texture->Bind();
    }

    mesh->DrawInstanced();
}

void InstancedObject::setTexture(Texture* tex) {
    hasTexture = true;
    texture = tex;
}
