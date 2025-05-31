#include "MaterialManager.h"

std::unordered_map<std::string, std::unique_ptr<Material>> MaterialManager::materials;

void MaterialManager::create(const std::string& name) {
    materials[name] = std::make_unique<Material>();
}

void MaterialManager::create(const std::string& name, const glm::vec3& color, float shininess) {
    auto mat = std::make_unique<Material>();
    mat->color = color;
    mat->shininess = shininess;
    materials[name] = std::move(mat);
}

void MaterialManager::setTexture(const std::string& name, Texture* texture) {
    if (materials.count(name)) {
        materials[name]->setTexture(texture);
    }
}

Material* MaterialManager::get(const std::string& name) {
    auto it = materials.find(name);
    if (it != materials.end()) {
        return it->second.get();
    }
    return nullptr;
}

void MaterialManager::clear() {
    materials.clear();
}

void MaterialManager::init()
{
	// Initialize default materials
	create("unlit", glm::vec3(1.0f, 1.0f, 1.0f), 0.0f); // Unlit material
	create("default", glm::vec3(1.0f, 1.0f, 1.0f), 32.0f); // Default material
	create("ground", glm::vec3(0.4f, 0.4f, 0.4f), 16.0f); // Ground material
    create3DPrinterMaterial();
}

Material* MaterialManager::create3DPrinterMaterial()
{
	auto material = std::make_unique<Material>();
	material->color = glm::vec3(0.4f, 0.4f, 0.4f); // Default color for 3D printer material
	material->shininess = 32.0f; // Default shininess
	material->ambientStrength = 0.2f; // Ambient strength for 3D printer material
	material->specularStrength = 0.5f; // Specular strength for 3D printer material
	materials["3DPrinter"] = std::move(material);
	return materials["3DPrinter"].get();
}


