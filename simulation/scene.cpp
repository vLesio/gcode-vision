#include "scene.h"

void Scene::add(SceneObject* obj) {
    objects.push_back(obj);
}

void Scene::remove(SceneObject* obj) {
    auto it = std::find(objects.begin(), objects.end(), obj);
    if (it != objects.end()) {
        delete* it; 
        objects.erase(it); 
    }
}

void Scene::addInstanced(InstancedObject* obj) {
    instancedObjects.push_back(obj);
}

void Scene::removeInstanced(InstancedObject* obj) {
    auto it = std::find(instancedObjects.begin(), instancedObjects.end(), obj);
    if (it != instancedObjects.end()) {
        delete* it;
        instancedObjects.erase(it);
    }
}

void Scene::Draw(Shader& regularShader, Shader& instancedShader) {
    for (auto& obj : objects)
        obj->Draw(regularShader);

    for (auto& iobj : instancedObjects)
        iobj->Draw(instancedShader);
}

void Scene::Delete() {
    for (auto& obj : objects)
        delete obj;

    for (auto& iobj : instancedObjects)
        delete iobj;

    objects.clear();
    instancedObjects.clear();
}
