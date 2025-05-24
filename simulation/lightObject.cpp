#include "LightObject.h"
#include <glm/gtc/type_ptr.hpp>

LightObject::LightObject(glm::vec3 color)
    : SceneObject(createLightCubeMesh(color)), lightColor(color) {
}

glm::vec3 LightObject::getLightColor() const {
    return lightColor;
}

glm::vec3 LightObject::getWorldPosition() const {
    glm::mat4 global = getGlobalMatrix();
    return glm::vec3(global[3]); 
}

void LightObject::uploadToShader(Shader& shader) const {
    shader.Activate();
    glUniform3fv(glGetUniformLocation(shader.ID, "lightPos"), 1, glm::value_ptr(getWorldPosition()));
    glUniform3fv(glGetUniformLocation(shader.ID, "lightColor"), 1, glm::value_ptr(lightColor));
}

std::unique_ptr<Mesh> LightObject::createLightCubeMesh(glm::vec3 color) {
    float r = color.r;
    float g = color.g;
    float b = color.b;

    GLfloat vertices[] = {
        // pos           // color          // UV
        -0.1f, -0.1f,  0.1f,   r, g, b,   0, 0,
         0.1f, -0.1f,  0.1f,   r, g, b,   1, 0,
         0.1f,  0.1f,  0.1f,   r, g, b,   1, 1,
        -0.1f,  0.1f,  0.1f,   r, g, b,   0, 1,

        -0.1f, -0.1f, -0.1f,   r, g, b,   0, 0,
         0.1f, -0.1f, -0.1f,   r, g, b,   1, 0,
         0.1f,  0.1f, -0.1f,   r, g, b,   1, 1,
        -0.1f,  0.1f, -0.1f,   r, g, b,   0, 1,

        -0.1f, -0.1f, -0.1f,   r, g, b,   0, 0,
        -0.1f, -0.1f,  0.1f,   r, g, b,   1, 0,
        -0.1f,  0.1f,  0.1f,   r, g, b,   1, 1,
        -0.1f,  0.1f, -0.1f,   r, g, b,   0, 1,

         0.1f, -0.1f,  0.1f,   r, g, b,   0, 0,
         0.1f, -0.1f, -0.1f,   r, g, b,   1, 0,
         0.1f,  0.1f, -0.1f,   r, g, b,   1, 1,
         0.1f,  0.1f,  0.1f,   r, g, b,   0, 1,

        -0.1f, -0.1f, -0.1f,   r, g, b,   0, 0,
         0.1f, -0.1f, -0.1f,   r, g, b,   1, 0,
         0.1f, -0.1f,  0.1f,   r, g, b,   1, 1,
        -0.1f, -0.1f,  0.1f,   r, g, b,   0, 1,

        -0.1f,  0.1f,  0.1f,   r, g, b,   0, 0,
         0.1f,  0.1f,  0.1f,   r, g, b,   1, 0,
         0.1f,  0.1f, -0.1f,   r, g, b,   1, 1,
        -0.1f,  0.1f, -0.1f,   r, g, b,   0, 1
    };

    GLuint indices[] = {
        0,1,2,2,3,0,
        4,5,6,6,7,4,
        8,9,10,10,11,8,
        12,13,14,14,15,12,
        16,17,18,18,19,16,
        20,21,22,22,23,20
    };

    return std::make_unique<Mesh>(vertices, sizeof(vertices), indices, sizeof(indices) / sizeof(GLuint));
}
