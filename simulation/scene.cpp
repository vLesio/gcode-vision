#include "Scene.h"

void Scene::add(SceneObject* obj) {
    objects.push_back(obj);
}

void Scene::addInstanced(InstancedObject* obj) {
    instancedObjects.push_back(obj);
}

void Scene::Draw(Shader& shader) {
    for (auto& obj : objects)
        obj->Draw(shader);
    for (auto& iobj : instancedObjects)
        iobj->Draw(shader);
}

void Scene::Delete() {
    for (auto& obj : objects) {
        obj->mesh->Delete();
        delete obj->mesh;
        delete obj;
    }
    for (auto& iobj : instancedObjects) {
        iobj->mesh->Delete();
        delete iobj->mesh;
        delete iobj;
    }
    objects.clear();
    instancedObjects.clear();
}
