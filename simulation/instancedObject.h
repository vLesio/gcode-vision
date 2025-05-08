#ifndef INSTANCED_OBJECT_H
#define INSTANCED_OBJECT_H

#include <vector>
#include <glm/glm.hpp>
#include "InstancedMesh.h"
#include "Texture.h"
#include "shader.h"

class InstancedObject {
public:
    InstancedMesh* mesh;
    std::vector<glm::vec3> instancePositions;
    std::vector<float> instanceScales;

    InstancedObject(InstancedMesh* mesh);
    void updateInstances();
    void Draw(Shader& shader);
};

#endif
