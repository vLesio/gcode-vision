#ifndef INSTANCED_OBJECT_H
#define INSTANCED_OBJECT_H

#include <vector>
#include <glm/glm.hpp>
#include "instancedMesh.h"
#include "shader.h"

class InstancedObject {
public:
    InstancedObject(InstancedMesh* mesh);
    InstancedMesh* mesh;

    void setInstances(const std::vector<glm::vec3>& positions,
        const std::vector<glm::vec3>& scales,
        const std::vector<glm::quat>& rotations);
    void updateInstances();

    void Draw(Shader& shader);

private:
    std::vector<glm::vec3> instancePositions;
    std::vector<glm::vec3> instanceScales;
    std::vector<glm::quat> instanceRotations;
};


#endif
