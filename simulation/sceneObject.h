#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <vector>
#include <glm/fwd.hpp>

#include "material.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "shader.h"

class SceneObject {
public:
    std::unique_ptr<Mesh> mesh;
	Material* material = nullptr; // SceneObject is not responsible for deleting the material!!!
    MatrixStack localTransform;
    SceneObject* parent = nullptr;    
    std::vector<SceneObject*> children;

    SceneObject(std::unique_ptr<Mesh> mesh);
    ~SceneObject();

    glm::mat4 getGlobalMatrix() const;
    void Draw(Shader& shader) const;

    void setMaterial(Material* mat);
	void setParent(SceneObject* newParent, bool keepGlobalTransform = false);
    void addChild(SceneObject* child);
    void removeChild(SceneObject* child);
};

#endif
