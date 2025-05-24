#include "instancedObject.h"
#include "Primitives.h"
#include "Mesh.h"

SceneObject* Primitives::createUnitCube() {
    GLfloat vertices[] = {
        // Front (+Z)
        -0.5f, -0.5f,  0.5f,   0, 0, 1,   0, 0,
         0.5f, -0.5f,  0.5f,   0, 0, 1,   1, 0,
         0.5f,  0.5f,  0.5f,   0, 0, 1,   1, 1,
        -0.5f,  0.5f,  0.5f,   0, 0, 1,   0, 1,

        // Back (-Z)
         0.5f, -0.5f, -0.5f,   0, 0, -1,   0, 0,
        -0.5f, -0.5f, -0.5f,   0, 0, -1,   1, 0,
        -0.5f,  0.5f, -0.5f,   0, 0, -1,   1, 1,
         0.5f,  0.5f, -0.5f,   0, 0, -1,   0, 1,

         // Left (-X)
         -0.5f, -0.5f, -0.5f,  -1, 0, 0,   0, 0,
         -0.5f, -0.5f,  0.5f,  -1, 0, 0,   1, 0,
         -0.5f,  0.5f,  0.5f,  -1, 0, 0,   1, 1,
         -0.5f,  0.5f, -0.5f,  -1, 0, 0,   0, 1,

         // Right (+X)
          0.5f, -0.5f,  0.5f,   1, 0, 0,   0, 0,
          0.5f, -0.5f, -0.5f,   1, 0, 0,   1, 0,
          0.5f,  0.5f, -0.5f,   1, 0, 0,   1, 1,
          0.5f,  0.5f,  0.5f,   1, 0, 0,   0, 1,

          // Bottom (-Y)
          -0.5f, -0.5f, -0.5f,   0, -1, 0,   0, 0,
           0.5f, -0.5f, -0.5f,   0, -1, 0,   1, 0,
           0.5f, -0.5f,  0.5f,   0, -1, 0,   1, 1,
          -0.5f, -0.5f,  0.5f,   0, -1, 0,   0, 1,

          // Top (+Y)
          -0.5f,  0.5f,  0.5f,   0, 1, 0,   0, 0,
           0.5f,  0.5f,  0.5f,   0, 1, 0,   1, 0,
           0.5f,  0.5f, -0.5f,   0, 1, 0,   1, 1,
          -0.5f,  0.5f, -0.5f,   0, 1, 0,   0, 1
    };

    GLuint indices[] = {
        0, 1, 2, 2, 3, 0,        // Front
        4, 5, 6, 6, 7, 4,        // Back
        8, 9,10,10,11, 8,        // Left
        12,13,14,14,15,12,       // Right
        16,17,18,18,19,16,       // Bottom
        20,21,22,22,23,20        // Top
    };

    auto cubeMesh = std::make_unique<Mesh>(vertices, sizeof(vertices), indices, sizeof(indices) / sizeof(GLuint));
    return new SceneObject(std::move(cubeMesh));
}

InstancedObject* Primitives::createInstancedCube() {
    // pos(x, y, z) + normal(x, y, z)
    GLfloat vertices[] = {
        // Front (+Z)
        -0.5f, -0.5f,  0.5f,   0,  0,  1,
         0.5f, -0.5f,  0.5f,   0,  0,  1,
         0.5f,  0.5f,  0.5f,   0,  0,  1,
        -0.5f,  0.5f,  0.5f,   0,  0,  1,

        // Back (-Z)
         0.5f, -0.5f, -0.5f,   0,  0, -1,
        -0.5f, -0.5f, -0.5f,   0,  0, -1,
        -0.5f,  0.5f, -0.5f,   0,  0, -1,
         0.5f,  0.5f, -0.5f,   0,  0, -1,

         // Left (-X)
         -0.5f, -0.5f, -0.5f,  -1,  0,  0,
         -0.5f, -0.5f,  0.5f,  -1,  0,  0,
         -0.5f,  0.5f,  0.5f,  -1,  0,  0,
         -0.5f,  0.5f, -0.5f,  -1,  0,  0,

         // Right (+X)
          0.5f, -0.5f,  0.5f,   1,  0,  0,
          0.5f, -0.5f, -0.5f,   1,  0,  0,
          0.5f,  0.5f, -0.5f,   1,  0,  0,
          0.5f,  0.5f,  0.5f,   1,  0,  0,

          // Bottom (-Y)
          -0.5f, -0.5f, -0.5f,   0, -1,  0,
           0.5f, -0.5f, -0.5f,   0, -1,  0,
           0.5f, -0.5f,  0.5f,   0, -1,  0,
          -0.5f, -0.5f,  0.5f,   0, -1,  0,

          // Top (+Y)
          -0.5f,  0.5f,  0.5f,   0,  1,  0,
           0.5f,  0.5f,  0.5f,   0,  1,  0,
           0.5f,  0.5f, -0.5f,   0,  1,  0,
          -0.5f,  0.5f, -0.5f,   0,  1,  0
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

    auto mesh = std::make_unique<InstancedMesh>(vertices, sizeof(vertices), indices, sizeof(indices) / sizeof(GLuint));
    return new InstancedObject(std::move(mesh));
}

InstancedObject* Primitives::createDirectionalCube() {
    // pos(x, y, z) + normal(x, y, z)
    GLfloat vertices[] = {
        // Front (+Z)
        0.0f, -0.5f,  0.5f,   0,  0,  1,
        1.0f, -0.5f,  0.5f,   0,  0,  1,
        1.0f,  0.5f,  0.5f,   0,  0,  1,
        0.0f,  0.5f,  0.5f,   0,  0,  1,

        // Back (-Z)
        1.0f, -0.5f, -0.5f,   0,  0, -1,
        0.0f, -0.5f, -0.5f,   0,  0, -1,
        0.0f,  0.5f, -0.5f,   0,  0, -1,
        1.0f,  0.5f, -0.5f,   0,  0, -1,

        // Left (-X)
        0.0f, -0.5f, -0.5f,  -1,  0,  0,
        0.0f, -0.5f,  0.5f,  -1,  0,  0,
        0.0f,  0.5f,  0.5f,  -1,  0,  0,
        0.0f,  0.5f, -0.5f,  -1,  0,  0,

        // Right (+X)
        1.0f, -0.5f,  0.5f,   1,  0,  0,
        1.0f, -0.5f, -0.5f,   1,  0,  0,
        1.0f,  0.5f, -0.5f,   1,  0,  0,
        1.0f,  0.5f,  0.5f,   1,  0,  0,

        // Bottom (-Y)
        0.0f, -0.5f, -0.5f,   0, -1,  0,
        1.0f, -0.5f, -0.5f,   0, -1,  0,
        1.0f, -0.5f,  0.5f,   0, -1,  0,
        0.0f, -0.5f,  0.5f,   0, -1,  0,

        // Top (+Y)
        0.0f,  0.5f,  0.5f,   0,  1,  0,
        1.0f,  0.5f,  0.5f,   0,  1,  0,
        1.0f,  0.5f, -0.5f,   0,  1,  0,
        0.0f,  0.5f, -0.5f,   0,  1,  0
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

    auto mesh = std::make_unique<InstancedMesh>(vertices, sizeof(vertices), indices, sizeof(indices) / sizeof(GLuint));
    return new InstancedObject(std::move(mesh));
}



SceneObject* Primitives::createPlane(float size) {
    float half = size / 2.0f;

    // pos(x,y,z), normal(x,y,z), UV(u,v)
    GLfloat vertices[] = {
        -half, 0.0f, -half,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
         half, 0.0f, -half,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
         half, 0.0f,  half,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        -half, 0.0f,  half,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f
    };

    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    auto mesh = std::make_unique<Mesh>(vertices, sizeof(vertices), indices, sizeof(indices) / sizeof(GLuint));
    return new SceneObject(std::move(mesh));
}

float M_PI = 3.14159265358979323846f;

InstancedObject* Primitives::createDirectedCylinder(int segments, float radius, float length) {
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

	// Side vertices
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        float y = cos(angle) * radius;
        float z = sin(angle) * radius;

        // start of the cylinder (x = 0)
        vertices.push_back(0.0f);
        vertices.push_back(y);
        vertices.push_back(z);
        vertices.push_back(0.0f);
        vertices.push_back(y / radius);
        vertices.push_back(z / radius);

        // end of the cylinder (x = length)
        vertices.push_back(length);
        vertices.push_back(y);
        vertices.push_back(z);
        vertices.push_back(0.0f);
        vertices.push_back(y / radius);
        vertices.push_back(z / radius);
    }

    // Sides indices
    for (int i = 0; i < segments * 2; i += 2) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);

        indices.push_back(i + 1);
        indices.push_back(i + 3);
        indices.push_back(i + 2);
    }

    // Center points for caps
    GLuint centerStartIndex = vertices.size() / 6;
    vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f); // pos
    vertices.push_back(-1.0f); vertices.push_back(0.0f); vertices.push_back(0.0f); // normal

    GLuint centerEndIndex = centerStartIndex + 1;
    vertices.push_back(length); vertices.push_back(0.0f); vertices.push_back(0.0f); // pos
    vertices.push_back(1.0f); vertices.push_back(0.0f); vertices.push_back(0.0f); // normal

    // Cap vertices
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        float y = cos(angle) * radius;
        float z = sin(angle) * radius;

        // start cap
        vertices.push_back(0.0f);
        vertices.push_back(y);
        vertices.push_back(z);
        vertices.push_back(-1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);

        // end cap
        vertices.push_back(length);
        vertices.push_back(y);
        vertices.push_back(z);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
    }

	// Cap indices
    GLuint startRing = centerEndIndex + 1;
    for (int i = 0; i < segments; ++i) {
        // start cap
        indices.push_back(centerStartIndex);
        indices.push_back(startRing + i * 2);
        indices.push_back(startRing + ((i + 1) % segments) * 2);

        // end cap
        indices.push_back(centerEndIndex);
        indices.push_back(startRing + ((i + 1) % segments) * 2 + 1);
        indices.push_back(startRing + i * 2 + 1);
    }

    // Create mesh
    auto mesh = std::make_unique<InstancedMesh>(
        vertices.data(),
        vertices.size() * sizeof(GLfloat),
        indices.data(),
        indices.size()
    );

    return new InstancedObject(std::move(mesh));
}

SceneObject* Primitives::createConeMarker(float radius, float height, int segments) {
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    // --- Apex of the cone ---
    vertices.insert(vertices.end(), {
        0.0f, 0.0f, 0.0f,     // Position
        0.0f, -1.0f, 0.0f,    // Normal (placeholder; not used)
        0.0f, 0.0f            // UV (optional)
        });

    // --- Side ring vertices ---
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        glm::vec3 position(x, height, z);
        glm::vec3 normal = glm::normalize(glm::vec3(x, radius / height, z)); // Approx. side normal

        vertices.insert(vertices.end(), {
            position.x, position.y, position.z,
            normal.x, normal.y, normal.z,
            0.0f, 0.0f // UV (optional)
            });
    }

    // --- Side faces indices ---
    for (int i = 1; i <= segments; ++i) {
        indices.push_back(0);       // Apex
        indices.push_back(i);       // Current base vertex
        indices.push_back(i + 1);   // Next base vertex
    }

    // --- Center vertex for the base ---
    int baseCenterIndex = static_cast<int>(vertices.size() / 8);
    vertices.insert(vertices.end(), {
        0.0f, height, 0.0f,     // Position
        0.0f, 1.0f, 0.0f,       // Normal pointing up
        0.0f, 0.0f              // UV (optional)
        });

    // --- Base fan indices ---
    for (int i = 1; i <= segments; ++i) {
        indices.push_back(baseCenterIndex);
        indices.push_back(i + 1);
        indices.push_back(i);
    }

    auto mesh = std::make_unique<Mesh>(
        vertices.data(),
        vertices.size() * sizeof(GLfloat),
        indices.data(),
        indices.size()
    );

    return new SceneObject(std::move(mesh));
}
