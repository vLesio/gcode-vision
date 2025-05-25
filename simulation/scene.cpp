#include "scene.h"

#include "materialManager.h"

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

void Scene::addUnlit(SceneObject* obj) {
	if (obj->material == nullptr) {
		obj->setMaterial(MaterialManager::get("unlit"));
	}
	unlitObjects.push_back(obj);
}
void Scene::removeUnlit(SceneObject* obj) {
	auto it = std::find(unlitObjects.begin(), unlitObjects.end(), obj);
	if (it != unlitObjects.end()) {
		delete* it;
		unlitObjects.erase(it);
	}
}

void Scene::Draw(Shader& regularShader, Shader& instancedShader, Shader& unlitShader) {
    for (auto& obj : objects)
        obj->Draw(regularShader);

    for (auto& iobj : instancedObjects)
        iobj->Draw(instancedShader);

	for (auto& uobj : unlitObjects)
		uobj->Draw(unlitShader);
}

void Scene::Delete() {
    for (auto& obj : objects)
        delete obj;

    for (auto& iobj : instancedObjects)
        delete iobj;

	for (auto& uobj : unlitObjects)
		delete uobj;

    objects.clear();
    instancedObjects.clear();
	unlitObjects.clear();
}
