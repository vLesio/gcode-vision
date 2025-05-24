#ifndef LIGHT_OBJECT_H
#define LIGHT_OBJECT_H
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SceneObject.h"
#include <glm/glm.hpp>

class LightObject : public SceneObject {
public:
    LightObject(glm::vec3 color = glm::vec3(1.0f));

    void uploadToShader(Shader& shader) const;
    static std::unique_ptr<Mesh> createLightCubeMesh();

    glm::vec3 getLightColor() const;

    glm::vec3 getWorldPosition() const;

private:
    glm::vec3 lightColor;
};

#endif
