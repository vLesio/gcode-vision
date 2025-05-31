#include "SceneObject.h"
#include <glm/gtc/type_ptr.hpp>

SceneObject::SceneObject(std::unique_ptr<Mesh> mesh) : mesh(std::move(mesh)) {}

SceneObject::~SceneObject() {
    for (auto* child : children)
        delete child;
}

glm::mat4 SceneObject::getGlobalMatrix() const {
    if (parent)
        return parent->getGlobalMatrix() * localTransform.top();
    return localTransform.top(); 
}

void SceneObject::Draw(Shader& shader) const {
	shader.Activate();
    glm::mat4 model = getGlobalMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

    if (material) {
        material->apply(shader);
    }
    else if (mesh && !material){
        std::cerr << "[ERROR] material is null but mesh is set in InstancedObject!\n";
        return;
    }

    if (mesh)
        mesh->Draw();

    for (SceneObject* child : children)
        child->Draw(shader);
}

void SceneObject::setMaterial(Material* mat) {
    if (mat == nullptr)
    {
		std::cerr << "[SceneObject] Attempted to set null material!" << std::endl;
		return;
    }

    material = mat;
}

void SceneObject::setMaterialApplyToChildren(Material* mat) {
    if (mat == nullptr) {
        std::cerr << "[SceneObject] Attempted to set null material!" << std::endl;
        return;
    }
    material = mat;
	for (auto child : children) {
        child->setMaterialApplyToChildren(mat);
	}
}


void SceneObject::setParent(SceneObject* newParent, bool keepGlobalTransform) {
    if (keepGlobalTransform && newParent) {
        glm::mat4 global = getGlobalMatrix();
        glm::mat4 parentGlobal = newParent->getGlobalMatrix();
        localTransform.setTop(glm::inverse(parentGlobal) * global);
    }

    if (parent) {
        auto it = std::find(parent->children.begin(), parent->children.end(), this);
        if (it != parent->children.end()) {
            parent->children.erase(it);
        }
    }

    parent = newParent;

    if (parent) {
        if (std::find(parent->children.begin(), parent->children.end(), this) == parent->children.end()) {
            parent->children.push_back(this);
        }
    }
}

void SceneObject::addChild(SceneObject* child) {
    if (child) {
        child->setParent(this);
    }
}

void SceneObject::removeChild(SceneObject* child) {
    if (!child) return;

    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->parent = nullptr;
        children.erase(it);
    }
}