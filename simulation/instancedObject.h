#ifndef INSTANCED_OBJECT_H
#define INSTANCED_OBJECT_H

#include <vector>
#include <glm/glm.hpp>
#include "instancedMesh.h"
#include "shader.h"

class InstancedObject {
public:
    InstancedObject(InstancedMesh* mesh);

    void setInstances(const std::vector<glm::vec3>& positions, const std::vector<float>& scales);
    void updateInstances();

    void Draw(Shader& shader);

    InstancedMesh* mesh = nullptr;

private:
    std::vector<glm::vec3> instancePositions;
    std::vector<float> instanceScales;
};

#endif
