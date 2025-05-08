#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <vector>
#include <glm/fwd.hpp>

#include "Mesh.h"
#include "MatrixStack.h"
#include "texture.h"
#include "shader.h"

class SceneObject {
public:
    Mesh* mesh;
    Texture* texture = nullptr;
    bool hasTexture = false;

    MatrixStack localTransform;        
    SceneObject* parent = nullptr;    
    std::vector<SceneObject*> children;

    SceneObject(Mesh* mesh);

    glm::mat4 getGlobalMatrix() const;
    void Draw(Shader& shader) const;
    void setTexture(Texture* tex);
	void setParent(SceneObject* newParent, bool keepGlobalTransform = false);
    void addChild(SceneObject* child);
    void removeChild(SceneObject* child);
};

#endif
