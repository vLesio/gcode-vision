#ifndef INSTANCED_MESH_H
#define INSTANCED_MESH_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp> 
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

class InstancedMesh {
public:
    VAO vao;
    VBO vbo;                
    EBO ebo;               
    VBO instancePosVBO;   
    VBO instanceScaleVBO;
    VBO instanceRotVBO;


    size_t indexCount;     
    size_t instanceCount = 0;

    InstancedMesh(GLfloat* vertices, size_t vSize, GLuint* indices, size_t iCount);

    void updateInstances(const std::vector<glm::vec3>& positions,
        const std::vector<glm::vec3>& scales,
        const std::vector<glm::quat>& rotations);


    void DrawInstanced();

    void Delete();
};



#endif
