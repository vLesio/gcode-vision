#include "Mesh.h"

Mesh::Mesh(GLfloat* vertices, size_t vSize, GLuint* indices, size_t iCount)
    : vbo(vertices, vSize, GL_STATIC_DRAW), ebo(indices, iCount * sizeof(GLuint), GL_STATIC_DRAW), indexCount(iCount)
{
    vao.Bind();
    ebo.Bind();

    vao.LinkAttribute(vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0); // Pos
    vao.LinkAttribute(vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Color
    vao.LinkAttribute(vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float))); // UV

    vao.Unbind();
}

void Mesh::Draw() {
    vao.Bind();
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void Mesh::Delete() {
    vao.Delete();
    vbo.Delete();
    ebo.Delete();
}