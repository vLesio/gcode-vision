#include "ShaderLoader.h"

std::unordered_map<std::string, std::unique_ptr<Shader>> ShaderLoader::shaders;

void ShaderLoader::load(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) {
    shaders[name] = std::make_unique<Shader>(vertexPath.c_str(), fragmentPath.c_str());
}

Shader* ShaderLoader::get(const std::string& name) {
    auto it = shaders.find(name);
    return (it != shaders.end()) ? it->second.get() : nullptr;
}

void ShaderLoader::clear() {
    shaders.clear();
}
