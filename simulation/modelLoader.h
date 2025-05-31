#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <string>
#include "SceneObject.h"
#include "Material.h"
#include "Texture.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

class ModelLoader {
public:
    static SceneObject* loadModel(const std::string& path);

private:
    static void processNode(aiNode* node, const aiScene* scene, SceneObject* parent, const std::string& directory);
    static std::unique_ptr<Mesh> processMesh(aiMesh* mesh);
    static Material* loadMaterial(aiMaterial* material, const aiScene* scene, const std::string& directory);
};

#endif
