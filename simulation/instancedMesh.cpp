#include <iostream>
#include <ostream>


#include "InstancedMesh.h"

InstancedMesh::InstancedMesh(GLfloat* vertices, size_t vSize, GLuint* indices, size_t iCount)
    : vbo(vertices, vSize, GL_STATIC_DRAW), ebo(indices, iCount * sizeof(GLuint), GL_STATIC_DRAW), indexCount(iCount)
{
    vao.Bind();
    ebo.Bind(); 

	vao.LinkAttribute(vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0); // only position needed, color set in shader
    vao.Unbind();
}

void InstancedMesh::updateInstances(const std::vector<glm::vec3>& positions,
    const std::vector<glm::vec3>& scales,
    const std::vector<glm::quat>& rotations) {
    instanceCount = positions.size();
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
    vao.LinkAttribute(instanceRotVBO, 5, 4, GL_FLOAT, sizeof(glm::quat), (void*)0); // quaternion = 4 floats
    glVertexAttribDivisor(5, 1);

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
