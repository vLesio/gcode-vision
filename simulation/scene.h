#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "sceneObject.h"
#include "instancedObject.h"
#include "shader.h"

class Scene {
public:
    void add(SceneObject* obj);
    void addInstanced(InstancedObject* obj);
    void removeInstanced(InstancedObject* obj);
    void Draw(Shader& regularShader, Shader& instancedShader);
    void Delete();

private:
    std::vector<SceneObject*> objects;
    std::vector<InstancedObject*> instancedObjects;
};

#endif
