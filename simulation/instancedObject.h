#ifndef INSTANCED_OBJECT_H
#define INSTANCED_OBJECT_H

#include <vector>
#include <glm/glm.hpp>
#include "InstancedMesh.h"
#include "Texture.h"
#include "shaderClass.h"

class InstancedObject {
public:
    InstancedMesh* mesh;
    std::vector<glm::vec3> instancePositions;
    std::vector<float> instanceScales;
    bool hasTexture = false;
    Texture* texture = nullptr;

    InstancedObject(InstancedMesh* mesh);
    void updateInstances();
    void Draw(Shader& shader);
    void setTexture(Texture* tex);
};

#endif
