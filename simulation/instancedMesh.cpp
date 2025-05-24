#include "InstancedMesh.h"
#include <iostream>

InstancedMesh::InstancedMesh(GLfloat* vertices, size_t vSize, GLuint* indices, size_t iCount)
    : vbo(vertices, vSize, GL_STATIC_DRAW), ebo(indices, iCount * sizeof(GLuint), GL_STATIC_DRAW), indexCount(iCount)
{
    vao.Bind();
    ebo.Bind();
	vao.LinkAttribute(vbo, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);                    // Position
	vao.LinkAttribute(vbo, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));  // Normal

    vao.Unbind();
}

InstancedMesh::~InstancedMesh() {
	Delete();
}

void InstancedMesh::uploadStaticInstances(const std::vector<glm::vec3>& positions,
    const std::vector<glm::vec3>& scales,
    const std::vector<glm::quat>& rotations) {
    mode = InstanceBufferMode::Static;
    instanceCount = positions.size();
    maxInstancesCount = 0;

    vao.Bind();

    instancePosVBO.Bind();
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_DYNAMIC_DRAW);
    vao.LinkAttribute(instancePosVBO, 3, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(3, 1);

    instanceScaleVBO.Bind();
    glBufferData(GL_ARRAY_BUFFER, scales.size() * sizeof(glm::vec3), scales.data(), GL_DYNAMIC_DRAW);
    vao.LinkAttribute(instanceScaleVBO, 4, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(4, 1);

    instanceRotVBO.Bind();
    glBufferData(GL_ARRAY_BUFFER, rotations.size() * sizeof(glm::quat), rotations.data(), GL_DYNAMIC_DRAW);
    vao.LinkAttribute(instanceRotVBO, 5, 4, GL_FLOAT, sizeof(glm::quat), (void*)0);
    glVertexAttribDivisor(5, 1);

    vao.Unbind();
}

void InstancedMesh::initializeDynamicBuffer(size_t maxInstances) {
    mode = InstanceBufferMode::Dynamic;
    vao.Bind();

    instancePosVBO.Bind();
    glBufferData(GL_ARRAY_BUFFER, maxInstances * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
    vao.LinkAttribute(instancePosVBO, 3, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(3, 1);

    instanceScaleVBO.Bind();
    glBufferData(GL_ARRAY_BUFFER, maxInstances * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
    vao.LinkAttribute(instanceScaleVBO, 4, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(4, 1);

    instanceRotVBO.Bind();
    glBufferData(GL_ARRAY_BUFFER, maxInstances * sizeof(glm::quat), nullptr, GL_DYNAMIC_DRAW);
    vao.LinkAttribute(instanceRotVBO, 5, 4, GL_FLOAT, sizeof(glm::quat), (void*)0);
    glVertexAttribDivisor(5, 1);

    maxInstancesCount = maxInstances;
    instanceCount = 0;

    vao.Unbind();
}

void InstancedMesh::appendInstance(const glm::vec3& position, const glm::vec3& scale, const glm::quat& rotation) {
    if (mode != InstanceBufferMode::Dynamic) {
        std::cerr << "[InstancedMesh] Cannot appendInstance: not in dynamic mode.\n";
        return;
    }

    if (instanceCount >= maxInstancesCount) {
        std::cerr << "[InstancedMesh] Maximum instance count exceeded!\n";
        return;
    }

    instancePosVBO.Bind();
    glBufferSubData(GL_ARRAY_BUFFER, instanceCount * sizeof(glm::vec3), sizeof(glm::vec3), &position);
    instanceScaleVBO.Bind();
    glBufferSubData(GL_ARRAY_BUFFER, instanceCount * sizeof(glm::vec3), sizeof(glm::vec3), &scale);
    instanceRotVBO.Bind();
    glBufferSubData(GL_ARRAY_BUFFER, instanceCount * sizeof(glm::quat), sizeof(glm::quat), &rotation);

    instanceCount++;
}

void InstancedMesh::uploadInstance(size_t index, const glm::vec3& position, const glm::vec3& scale, const glm::quat& rotation) {
    if (mode != InstanceBufferMode::Dynamic) {
        std::cerr << "[InstancedMesh] Cannot uploadInstance: not in dynamic mode.\n";
        return;
    }

    if (index >= instanceCount) {
        std::cerr << "[InstancedMesh] Instance index out of bounds!\n";
        return;
    }

    instancePosVBO.Bind();
    glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(glm::vec3), sizeof(glm::vec3), &position);
    instanceScaleVBO.Bind();
    glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(glm::vec3), sizeof(glm::vec3), &scale);
    instanceRotVBO.Bind();
    glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(glm::quat), sizeof(glm::quat), &rotation);
}

void InstancedMesh::resetDynamicBuffer() {
    if (mode != InstanceBufferMode::Dynamic) {
        std::cerr << "[InstancedMesh] Cannot reset: not in dynamic mode.\n";
        return;
    }

    instanceCount = 0;
}

void InstancedMesh::DrawInstanced() {
    vao.Bind();
    glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, instanceCount);
	vao.Unbind();
}

void InstancedMesh::Delete() {
    vao.Delete();
    vbo.Delete();
    ebo.Delete();
    instancePosVBO.Delete();
    instanceScaleVBO.Delete();
    instanceRotVBO.Delete();
}