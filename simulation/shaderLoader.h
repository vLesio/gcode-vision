#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <string>
#include <unordered_map>
#include <memory>
#include "Shader.h"

class ShaderLoader {
public:
    static void load(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
    static Shader* get(const std::string& name);
    static void clear();

private:
    static std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
};

#endif // SHADER_LOADER_H
