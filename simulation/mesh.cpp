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

Mesh* Mesh::createUnitCube() {
	// 24 unique vertices for right texturing
    // pos(3) + color(3) + UV(2)
    GLfloat vertices[] = {
        // Front face
        -0.5f, -0.5f,  0.5f,  1, 0, 0,  0, 0,
         0.5f, -0.5f,  0.5f,  0, 1, 0,  1, 0,
         0.5f,  0.5f,  0.5f,  0, 0, 1,  1, 1,
        -0.5f,  0.5f,  0.5f,  1, 1, 0,  0, 1,

        // Back face
        -0.5f, -0.5f, -0.5f,  1, 0, 1,  0, 0,
         0.5f, -0.5f, -0.5f,  0, 1, 1,  1, 0,
         0.5f,  0.5f, -0.5f,  1, 1, 1,  1, 1,
        -0.5f,  0.5f, -0.5f,  0, 0, 0,  0, 1,

        // Left face
        -0.5f, -0.5f, -0.5f,  1, 0, 0,  0, 0,
        -0.5f, -0.5f,  0.5f,  0, 1, 0,  1, 0,
        -0.5f,  0.5f,  0.5f,  0, 0, 1,  1, 1,
        -0.5f,  0.5f, -0.5f,  1, 1, 0,  0, 1,

        // Right face
         0.5f, -0.5f,  0.5f,  1, 0, 1,  0, 0,
         0.5f, -0.5f, -0.5f,  0, 1, 1,  1, 0,
         0.5f,  0.5f, -0.5f,  1, 1, 1,  1, 1,
         0.5f,  0.5f,  0.5f,  0, 0, 0,  0, 1,

         // Bottom face
         -0.5f, -0.5f, -0.5f,  1, 0, 0,  0, 0,
          0.5f, -0.5f, -0.5f,  0, 1, 0,  1, 0,
          0.5f, -0.5f,  0.5f,  0, 0, 1,  1, 1,
         -0.5f, -0.5f,  0.5f,  1, 1, 0,  0, 1,

         // Top face
         -0.5f,  0.5f,  0.5f,  1, 0, 1,  0, 0,
          0.5f,  0.5f,  0.5f,  0, 1, 1,  1, 0,
          0.5f,  0.5f, -0.5f,  1, 1, 1,  1, 1,
         -0.5f,  0.5f, -0.5f,  0, 0, 0,  0, 1
    };

    GLuint indices[] = {
        // Front
        0, 1, 2, 2, 3, 0,
        // Back
        4, 5, 6, 6, 7, 4,
        // Left
        8, 9,10,10,11, 8,
        // Right
		12,13,14,14,15,12,
		// Bottom
		16,17,18,18,19,16,
		// Top
		20,21,22,22,23,20
    };

    return new Mesh(vertices, sizeof(vertices), indices, sizeof(indices) / sizeof(GLuint));
}
