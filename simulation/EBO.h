#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <glad/glad.h>

class EBO {
public:
    GLuint ID;

    EBO() = default;
    EBO(GLuint* indices, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);

    void Bind();
    static void Unbind();
    void Delete();
};

#endif
