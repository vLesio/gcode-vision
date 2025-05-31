#ifndef MATERIAL_MANAGER_H
#define MATERIAL_MANAGER_H

#include <unordered_map>
#include <string>
#include <memory>
#include "material.h"

class MaterialManager {
public:
    static void create(const std::string& name);
    static void create(const std::string& name, const glm::vec3& color, float shininess = 32.0f);
    static void setTexture(const std::string& name, Texture* texture);
    static Material* get(const std::string& name);
    static void clear();
    static void init();

private:
    static std::unordered_map<std::string, std::unique_ptr<Material>> materials;
    static Material* create3DPrinterMaterial();
};

#endif
