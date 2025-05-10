#include "TextureLoader.h"

std::unordered_map<std::string, std::unique_ptr<Texture>> TextureLoader::textures;

void TextureLoader::load(const std::string& name, const std::string& path, GLenum texType, GLenum format, GLenum pixelType) {
    textures[name] = std::make_unique<Texture>(path.c_str(), texType, GL_TEXTURE0, format, pixelType);
}

Texture* TextureLoader::get(const std::string& name) {
    auto it = textures.find(name);
    return (it != textures.end()) ? it->second.get() : nullptr;
}

void TextureLoader::bindToShader(const std::string& name, Shader& shader, const std::string& uniformName, GLuint unitIndex) {
    Texture* tex = get(name);
    if (tex) {
        tex->texUnit(shader, uniformName.c_str(), unitIndex);
    }
}

void TextureLoader::clear() {
    for (auto& [_, tex] : textures) {
        tex->Delete();
    }
    textures.clear();
}
