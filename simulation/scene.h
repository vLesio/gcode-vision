#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "SceneObject.h"
#include "InstancedObject.h"

class Scene {
public:
    std::vector<SceneObject*> objects;
    std::vector<InstancedObject*> instancedObjects;

    void add(SceneObject* obj);
    void addInstanced(InstancedObject* obj);
    void Draw(Shader& shader);
    void Delete();
};

#endif
