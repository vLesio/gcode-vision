#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <string>
#include <unordered_map>
#include <memory>
#include "texture.h"

class TextureLoader {
public:
    static void load(const std::string& name, const std::string& path, GLenum texType = GL_TEXTURE_2D, GLenum format = GL_RGBA, GLenum pixelType = GL_UNSIGNED_BYTE);
    static Texture* get(const std::string& name);
    static void bindToShader(const std::string& name, Shader& shader, const std::string& uniformName, GLuint unitIndex);
    static void clear();

private:
    static std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
};

#endif // TEXTURE_LOADER_H
