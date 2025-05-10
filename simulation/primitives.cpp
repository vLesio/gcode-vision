#include "instancedObject.h"
#include "Primitives.h"
#include "Mesh.h"

SceneObject* Primitives::createUnitCube() {
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

    Mesh* cubeMesh = new Mesh(vertices, sizeof(vertices), indices, sizeof(indices) / sizeof(GLuint));

    return new SceneObject(cubeMesh);
}

InstancedObject* Primitives::createInstancedCube() {
    GLfloat vertices[] = {
        // Front face
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        // Back face
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
    };

    GLuint indices[] = {
        // Front
        0, 1, 2, 2, 3, 0,
        // Back
        5, 4, 7, 7, 6, 5,
        // Left
        4, 0, 3, 3, 7, 4,
        // Right
        1, 5, 6, 6, 2, 1,
        // Bottom
        4, 5, 1, 1, 0, 4,
        // Top
        3, 2, 6, 6, 7, 3
    };

    auto* mesh = new InstancedMesh(vertices, sizeof(vertices), indices, sizeof(indices) / sizeof(GLuint));
    return new InstancedObject(mesh);
}

InstancedObject* Primitives::createDirectionalCube() {
    GLfloat vertices[] = {
        // Front face
        0.0f, -0.5f,  0.5f,
        1.0f, -0.5f,  0.5f,
        1.0f,  0.5f,  0.5f,
        0.0f,  0.5f,  0.5f,

        // Back face
        0.0f, -0.5f, -0.5f,
        1.0f, -0.5f, -0.5f,
        1.0f,  0.5f, -0.5f,
        0.0f,  0.5f, -0.5f
    };

    GLuint indices[] = {
        // Front
        0, 1, 2, 2, 3, 0,
        // Back
        5, 4, 7, 7, 6, 5,
        // Left
        4, 0, 3, 3, 7, 4,
        // Right
        1, 5, 6, 6, 2, 1,
        // Bottom
        4, 5, 1, 1, 0, 4,
        // Top
        3, 2, 6, 6, 7, 3
    };

    auto* mesh = new InstancedMesh(vertices, sizeof(vertices), indices, sizeof(indices) / sizeof(GLuint));
    return new InstancedObject(mesh);
}


SceneObject* Primitives::createTexturedPlane(float size) {
    float half = size / 2.0f;

    // pos(x,y,z), color(r,g,b), UV(u,v)
    GLfloat vertices[] = {
        -half, 0.0f, -half,   0.2f, 0.2f, 0.2f,   0.0f, 0.0f,
         half, 0.0f, -half,   0.2f, 0.2f, 0.2f,   1.0f, 0.0f,
         half, 0.0f,  half,   0.2f, 0.2f, 0.2f,   1.0f, 1.0f,
        -half, 0.0f,  half,   0.2f, 0.2f, 0.2f,   0.0f, 1.0f
    };

    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    Mesh* mesh = new Mesh(vertices, sizeof(vertices), indices, sizeof(indices) / sizeof(GLuint));
    return new SceneObject(mesh);
}
