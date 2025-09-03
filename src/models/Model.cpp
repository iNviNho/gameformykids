

#include "Model.h"

#include "../textures/TextureLoader.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

Model::Model(const std::filesystem::path& modelPath) {
    loadModel(modelPath);
}

Model::Model(const std::filesystem::path& modelPath, const std::filesystem::path& texturePath) {
    loadModel(modelPath);
    loadSingleTexture(texturePath);
}

Model::Model(const std::filesystem::path& modelPath, Texture& texture) {
    loadModel(modelPath);
    loadSingleTexture(texture);
}

Model::Model(Mesh&& mesh) {
    meshes.push_back(std::move(mesh));
}

void Model::Draw(Shader& shader) const
{
    for(unsigned int i = 0; i < GetMeshes().size(); i++) {
        GetMeshes().at(i).Draw(shader);
    }
}

void Model::loadModel(const std::filesystem::path& modelPath)
{
    Assimp::Importer importer;
    // few other post processing options are
    //     aiProcess_GenNormals: creates normal vectors for each vertex if the model doesn’t
    // contain normal vectors
    // aiProcess_SplitLargeMeshes: splits large meshes into smaller sub-meshes which
    // is useful if your rendering has a maximum number of vertices allowed and can only process
    // smaller meshes.
    // aiProcess_OptimizeMeshes: does the reverse by trying to join several meshes into
    // one larger mesh, reducing drawing calls for optimization
    //
    // if aiProcess_Triangulate is removed, also Mesh.cpp has to be updated as each face could have potentially
    // more vertices
    const aiScene *scene = importer.ReadFile(modelPath.native(), aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    directory = modelPath.parent_path();

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    // process all the node’s meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    vertices.reserve(mesh->mNumVertices);
    std::vector<unsigned int> indices;
    indices.reserve(mesh->mNumFaces * 3);
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        // check if normals are present
        if (mesh->HasNormals()) {
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        } else {
            // TBA: Calculate normals
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        // assimp allows up to 8 different texture coordinates per vertex,
        // for now we only care abbout first
        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        } else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.emplace_back(vertex);
    }
    // assimp stores all the mesh’s faces in the mFaces array
    // each face is basically a triangle
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", textures);
        loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", textures);
    }

    return Mesh(std::move(vertices), std::move(indices), std::move(textures));
}

void Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string& typeName, std::vector<Texture>& textures) {
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        const std::filesystem::path str_path(str.C_Str());
        bool skip = false;
        for (unsigned int j = 0; j < texturesLoaded.size(); j++) {
            if (texturesLoaded[j].path == str_path) {
                textures.push_back(texturesLoaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            Texture texture;
            const std::filesystem::path fullPath = std::filesystem::path(this->directory) /= str_path;
            texture.id = TextureLoader::loadTexture(fullPath);
            texture.type = typeName;
            texture.path = str_path;
            textures.push_back(texture);
            texturesLoaded.push_back(texture);
        }
    }
}

void Model::loadSingleTexture(const std::filesystem::path& texturePath) {
    Texture texture;
    texture.id = TextureLoader::loadTexture(texturePath);
    texture.type = "texture_diffuse";
    texture.path = texturePath;
    texturesLoaded.push_back(texture);
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].getTextures().push_back(texture);
    }
}

void Model::loadSingleTexture(Texture& texture) {
    texturesLoaded.push_back(texture);
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes.at(i).getTextures().push_back(texture);
    }
}


