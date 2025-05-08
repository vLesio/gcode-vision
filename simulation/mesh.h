#ifndef MESH_H
#define MESH_H
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

class Mesh {
public:
    VAO vao;
    VBO vbo;
    EBO ebo;
    size_t indexCount;

    Mesh(GLfloat* vertices, size_t vSize, GLuint* indices, size_t iCount);
    void Draw();
    void Delete();
};
#endif
