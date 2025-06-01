#include "ModelLoader.h"
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

#include "TextureLoader.h"

std::unique_ptr<Mesh> ModelLoader::processMesh(aiMesh* mesh) {
    std::vector<float> vertices;
    std::vector<GLuint> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        // Position
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);

        // Normal
        if (mesh->HasNormals()) {
            vertices.push_back(mesh->mNormals[i].x);
            vertices.push_back(mesh->mNormals[i].y);
            vertices.push_back(mesh->mNormals[i].z);
        }
        else {
			std::cerr << "[ModelLoader] Mesh has no normals, using default (0, 1, 0)" << std::endl;
            vertices.insert(vertices.end(), { 0.0f, 1.0f, 0.0f }); // default up
        }

        // TexCoords
        if (mesh->mTextureCoords[0]) {
            vertices.push_back(mesh->mTextureCoords[0][i].x);
            vertices.push_back(mesh->mTextureCoords[0][i].y);
        }
        else {
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    return std::make_unique<Mesh>(vertices.data(), vertices.size() * sizeof(float), indices.data(), indices.size());
}

Material* ModelLoader::loadMaterial(aiMaterial* mat, const aiScene* scene, const std::string& directory) {
    aiColor3D color(1.0f, 1.0f, 1.0f);
    mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);

    Material* material = new Material();
    material->color = glm::vec3(glm::vec3(color.r, color.g, color.b));

    if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
        aiString str;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &str);
        std::string texturePath = directory + "/" + std::string(str.C_Str());

        if (std::filesystem::exists(texturePath)) {
			TextureLoader::load(str.C_Str(), texturePath, GL_TEXTURE_2D, GL_RGBA, GL_UNSIGNED_BYTE);

			Texture* tex = TextureLoader::get(str.C_Str());
            material->setTexture(tex);
        }
        else {
            std::cerr << "[ModelLoader] Texture file not found: " << texturePath << std::endl;
        }
    }
	else
    {
		std::cerr << "[ModelLoader] No diffuse texture found for material." << std::endl;
    }

    return material;
}

void ModelLoader::processNode(aiNode* node, const aiScene* scene, SceneObject* parent, const std::string& directory) {
    for (int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::unique_ptr<Mesh> newMesh = processMesh(mesh);

        SceneObject* obj = new SceneObject(std::move(newMesh));
        Material* mat = loadMaterial(scene->mMaterials[mesh->mMaterialIndex], scene, directory);
        obj->setMaterial(mat);

        parent->addChild(obj);
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene, parent->children[i], directory);
    }
}

SceneObject* ModelLoader::loadModel(const std::string& path) {
    Assimp::Importer importer;

	std::cout << "[ModelLoader] Loading model from: " << path << std::endl;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs);

    if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        std::cerr << "[ModelLoader] Error loading model: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    std::string directory = std::filesystem::path(path).parent_path().string();
    SceneObject* root = new SceneObject(nullptr); 

    processNode(scene->mRootNode, scene, root, directory);
    return root;
}
