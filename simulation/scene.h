#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "sceneObject.h"
#include "instancedObject.h"
#include "shader.h"

class Scene {
public:
    void add(SceneObject* obj);
	void remove(SceneObject* obj);

    void addInstanced(InstancedObject* obj);
    void removeInstanced(InstancedObject* obj);

    void addUnlit(SceneObject* obj);
	void removeUnlit(SceneObject* obj);

	void Draw(Shader& regularShader, Shader& instancedShader, Shader& unlitShader);
    void Delete();

private:
    std::vector<SceneObject*> objects;
    std::vector<SceneObject*> unlitObjects;
    std::vector<InstancedObject*> instancedObjects;
};

#endif
