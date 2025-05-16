#ifndef INSTANCED_MESH_H
#define INSTANCED_MESH_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

enum class InstanceBufferMode { Static, Dynamic };

class InstancedMesh {
public:
    InstancedMesh(GLfloat* vertices, size_t vSize, GLuint* indices, size_t iCount);
    ~InstancedMesh();

    void uploadStaticInstances(const std::vector<glm::vec3>& positions,
                               const std::vector<glm::vec3>& scales,
                               const std::vector<glm::quat>& rotations);

    void initializeDynamicBuffer(size_t maxInstances);
    void appendInstance(const glm::vec3& position, const glm::vec3& scale, const glm::quat& rotation);
    void uploadInstance(size_t index, const glm::vec3& position, const glm::vec3& scale, const glm::quat& rotation);
    void resetDynamicBuffer();
	InstanceBufferMode getInstanceMode() const { return mode; }

    void DrawInstanced();
    void Delete();

    size_t getInstanceCount() const { return instanceCount; }

private:
    VAO vao;
    VBO vbo, instancePosVBO, instanceScaleVBO, instanceRotVBO;
    EBO ebo;

    size_t indexCount = 0;
    size_t instanceCount = 0;
    size_t maxInstancesCount = 0;

    InstanceBufferMode mode = InstanceBufferMode::Static;
};
#endif
