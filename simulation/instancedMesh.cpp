#include "InstancedMesh.h"

InstancedMesh::InstancedMesh(GLfloat* vertices, size_t vSize, GLuint* indices, size_t iCount)
    : vbo(vertices, vSize, GL_STATIC_DRAW), ebo(indices, iCount * sizeof(GLuint), GL_STATIC_DRAW), indexCount(iCount)
{
    vao.Bind();
    ebo.Bind(); 

    vao.LinkAttribute(vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0); // pos
    vao.LinkAttribute(vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float))); // color
    vao.LinkAttribute(vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float))); // UV
    vao.Unbind();
}

void InstancedMesh::updateInstances(const std::vector<glm::vec3>& positions, const std::vector<float>& scales) {
    instanceCount = positions.size();
    vao.Bind();

    // Instance positions
    instancePosVBO.Bind();
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_DYNAMIC_DRAW);
    vao.LinkAttribute(instancePosVBO, 3, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(3, 1);

    // Instance scales
    instanceScaleVBO.Bind();
    glBufferData(GL_ARRAY_BUFFER, scales.size() * sizeof(float), scales.data(), GL_DYNAMIC_DRAW);
    vao.LinkAttribute(instanceScaleVBO, 4, 1, GL_FLOAT, sizeof(float), (void*)0);
    glVertexAttribDivisor(4, 1);

    vao.Unbind();
}

void InstancedMesh::DrawInstanced() {
    vao.Bind();
    glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, instanceCount);
}

void InstancedMesh::Delete() {
    vao.Delete();
    vbo.Delete();
    ebo.Delete();
    instancePosVBO.Delete();
    instanceScaleVBO.Delete();
}
