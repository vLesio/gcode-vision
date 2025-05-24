#include <glm/gtc/type_ptr.hpp>

#include "Material.h"


Material::Material()
    : texture(nullptr), color(glm::vec3(1.0f)), shininess(32.0f),
    specularStrength(0.3f), ambientStrength(0.6f), useTexture(false) {
}

Material::Material(Texture* texture, glm::vec3 color, float shininess)
    : texture(texture), color(color), shininess(shininess),
    specularStrength(0.3f), ambientStrength(0.6f),
    useTexture(texture != nullptr) {
}

void Material::setTexture(Texture* tex) {
    texture = tex;
    useTexture = (tex != nullptr);
}

void Material::apply(Shader& shader) const {
    shader.Activate();

    glUniform3fv(glGetUniformLocation(shader.ID, "objectColor"), 1, glm::value_ptr(color));
    glUniform1f(glGetUniformLocation(shader.ID, "shininess"), shininess);
    glUniform1f(glGetUniformLocation(shader.ID, "specularStrength"), specularStrength);
    glUniform1f(glGetUniformLocation(shader.ID, "ambientStrength"), ambientStrength);
    glUniform1f(glGetUniformLocation(shader.ID, "useTexture"), useTexture ? 1.0f : 0.0f);

    if (useTexture && texture) {
        texture->Bind();
    }
}