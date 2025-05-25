#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>
#include "Shader.h"
#include "Texture.h"

class Material {
public:
    Texture* texture = nullptr;
    glm::vec3 color = glm::vec3(1.0f);
    float shininess = 32.0f;
    float specularStrength = 0.4f;
    float ambientStrength = 0.6f;
    bool useTexture = false;
	float textureMixRatio = 1.0f; // Ratio for mixing texture with color 1 - full texture, 0 - full color

    Material();
	Material(Texture* texture, glm::vec3 color = glm::vec3(1.0f), float shininess = 32.0f);

    void apply(Shader& shader) const;
    void setTexture(Texture* tex);
};

#endif