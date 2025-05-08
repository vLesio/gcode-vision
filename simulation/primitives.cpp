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
    //pos(3) 
    GLfloat vertices[] = {
    -0.5f, -0.5f, -0.5f,  // 0
     0.5f, -0.5f, -0.5f,  // 1
     0.5f,  0.5f, -0.5f,  // 2
    -0.5f,  0.5f, -0.5f,  // 3
    -0.5f, -0.5f,  0.5f,  // 4
     0.5f, -0.5f,  0.5f,  // 5
     0.5f,  0.5f,  0.5f,  // 6
    -0.5f,  0.5f,  0.5f   // 7
    };


    GLuint indices[] = {
        // Front
        4, 5, 6, 6, 7, 4,
        // Back
        1, 0, 3, 3, 2, 1,
        // Left
        0, 4, 7, 7, 3, 0,
        // Right
        5, 1, 2, 2, 6, 5,
        // Bottom
        0, 1, 5, 5, 4, 0,
        // Top
        3, 7, 6, 6, 2, 3
    };

    InstancedMesh* mesh = new InstancedMesh(vertices, sizeof(vertices), indices, sizeof(indices) / sizeof(GLuint));
    return new InstancedObject(mesh);
}