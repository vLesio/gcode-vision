#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glad/glad.h>

class VBO {
public:
    GLuint ID;

    VBO();
    VBO(GLfloat* vertices, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);

    void Bind();
    static void Unbind();
    void Delete();
};

#endif
