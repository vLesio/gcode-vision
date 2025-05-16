#ifndef INSTANCED_OBJECT_H
#define INSTANCED_OBJECT_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "InstancedMesh.h"
#include "Shader.h"

class InstancedObject {
public:
    std::unique_ptr<InstancedMesh> mesh;

    InstancedObject(std::unique_ptr<InstancedMesh> mesh);

    void Draw(Shader& shader);

    // Static mode
    void setStaticInstances(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& scales, const std::vector<glm::quat>& rotations);
    void uploadStaticToGPU();

    // Dynamic mode
    void appendDynamicInstance(const glm::vec3& pos, const glm::vec3& scale, const glm::quat& rot);
    void updateDynamicInstance(size_t i, const glm::vec3& pos, const glm::vec3& scale, const glm::quat& rot);
    void reserveInstances(size_t maxCount);

    // Both
    void clear();

private:
    std::vector<glm::vec3> instancePositions;
    std::vector<glm::vec3> instanceScales;
    std::vector<glm::quat> instanceRotations;
};

#endif
