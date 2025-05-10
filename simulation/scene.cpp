#include "scene.h"

void Scene::add(SceneObject* obj) {
    objects.push_back(obj);
}

void Scene::addInstanced(InstancedObject* obj) {
    instancedObjects.push_back(obj);
}

void Scene::removeInstanced(InstancedObject* obj) {
    instancedObjects.erase(
        std::remove(instancedObjects.begin(), instancedObjects.end(), obj),
        instancedObjects.end()
    );
}

void Scene::Draw(Shader& regularShader, Shader& instancedShader) {
    for (auto& obj : objects)
        obj->Draw(regularShader);

    for (auto& iobj : instancedObjects)
        iobj->Draw(instancedShader);
}

void Scene::Delete() {
    for (auto& obj : objects) {
        if (obj->mesh) {
            obj->mesh->Delete();
            delete obj->mesh;
        }
        delete obj;
    }

    for (auto& iobj : instancedObjects) {
        if (iobj->mesh) {
            iobj->mesh->Delete();
            delete iobj->mesh;
        }
        delete iobj;
    }

    objects.clear();
    instancedObjects.clear();
}
