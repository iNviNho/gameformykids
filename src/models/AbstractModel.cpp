

#include "AbstractModel.h"

#include "../textures/TextureLoader.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include <utility>

AbstractModel::AbstractModel(const std::filesystem::path& modelPath) {
    loadModel(modelPath);
}

AbstractModel::AbstractModel(const std::filesystem::path& modelPath, const std::filesystem::path& texturePath) {
    loadModel(modelPath);
    loadSingleTexture(texturePath);
}

AbstractModel::AbstractModel(const std::filesystem::path& modelPath, Texture& texture) {
    loadModel(modelPath);
    loadSingleTexture(texture);
}

AbstractModel::AbstractModel(Mesh&& mesh) {
    meshes.push_back(std::move(mesh));
}

void AbstractModel::Draw(Shader& shader) {
    for(unsigned int i = 0; i < GetMeshes().size(); i++) {
        GetMeshes().at(i).Draw(shader);
    }
}

void AbstractModel::loadModel(const std::filesystem::path& modelPath, bool flipUVs)
{
    // few other post processing options are
    //     aiProcess_GenNormals: creates normal vectors for each vertex if the model doesn't
    // contain normal vectors
    // aiProcess_SplitLargeMeshes: splits large meshes into smaller sub-meshes which
    // is useful if your rendering has a maximum number of vertices allowed and can only process
    // smaller meshes.
    // aiProcess_OptimizeMeshes: does the reverse by trying to join several meshes into
    // one larger mesh, reducing drawing calls for optimization
    //
    // if aiProcess_Triangulate is removed, also Mesh.cpp has to be updated as each face could have potentially
    // more vertices
    unsigned int flags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices;
    if (flipUVs) flags |= aiProcess_FlipUVs;
    flipTexturesVertically = !flipUVs;
    pScene = importer.ReadFile(modelPath.native(), flags);

    if(!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    // So far commented out as it gives wrong position on the first model
    // pScene->mRootNode->mTransformation.Inverse();

    directory = modelPath.parent_path();

    processNode(pScene->mRootNode, pScene);
}

void AbstractModel::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

//void AbstractModel::processNode(const aiScene *scene) {
//
//    // process all the scene's meshes
//    for(unsigned int i = 0; i < scene->mNumMeshes; i++)
//    {
//        meshes.push_back(processMesh(scene->mMeshes[i], scene));
//    }
//}



Mesh AbstractModel::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    vertices.reserve(mesh->mNumVertices);
    std::vector<unsigned int> indices;
    indices.reserve(mesh->mNumFaces * 3);
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        glm::vec3 position(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        glm::vec3 normal = mesh->HasNormals()
            ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
            : glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec2 texCoords = mesh->mTextureCoords[0]
            ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
            : glm::vec2(0.0f, 0.0f);

        vertices.emplace_back(position, normal, texCoords);
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

        // Enable to debug what kind of texture types do I receive
        //static const std::vector<aiTextureType> types = {
        //    aiTextureType_DIFFUSE,
        //    aiTextureType_SPECULAR,
        //    aiTextureType_AMBIENT,
        //    aiTextureType_EMISSIVE,
        //    aiTextureType_HEIGHT,
        //    aiTextureType_NORMALS,
        //    aiTextureType_SHININESS,
        //    aiTextureType_OPACITY,
        //    aiTextureType_DISPLACEMENT,
        //    aiTextureType_LIGHTMAP,
        //    aiTextureType_REFLECTION,
        //    aiTextureType_BASE_COLOR,
        //    aiTextureType_UNKNOWN
        //};
        //
        //for (auto type : types) {
        //    unsigned int count = material->GetTextureCount(type);
        //    if (count > 0) {
        //        std::cout << "Type " << type << " has " << count << " textures\n";
        //    }
        //}

        loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", textures);
        // TODO: Specular texture is disabled for now
        //loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", textures);
        loadMaterialTextures(material, aiTextureType_OPACITY, "texture_opacity", textures);
    } else {
        Log::logWarning("Mesh has no material assigned to it.");
    }

    return Mesh(
        std::move(vertices),
        std::move(indices),
        std::move(textures),
        {}
    );
}

void AbstractModel::loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string& typeName, std::vector<Texture>& textures) {
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string rawTexturePath(str.C_Str());
        std::replace(rawTexturePath.begin(), rawTexturePath.end(), '\\', '/');
        const std::filesystem::path textureFilename(rawTexturePath);
        const std::filesystem::path textureFilenameFullpath = std::filesystem::path(this->directory) /= textureFilename;
        if (!std::filesystem::exists(textureFilenameFullpath)) {
            Log::logError("Texture path does not exist on filesystem: " + textureFilenameFullpath.string());
        }
        bool skip = false;
        for (unsigned int j = 0; j < texturesLoaded.size(); j++) {
            if (texturesLoaded[j].path == textureFilename) {
                textures.push_back(texturesLoaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            Texture texture;
            texture.id = TextureLoader::loadTexture(textureFilenameFullpath, flipTexturesVertically);
            texture.type = typeName;
            texture.path = textureFilename;
            textures.push_back(texture);
            texturesLoaded.push_back(texture);
        }
    }
}

void AbstractModel::loadSingleTexture(const std::filesystem::path& texturePath) {
    Texture texture;
    texture.id = TextureLoader::loadTexture(texturePath);
    texture.type = "texture_diffuse";
    texture.path = texturePath;
    texturesLoaded.push_back(texture);
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].getTextures().push_back(texture);
    }
}

void AbstractModel::loadSingleTexture(Texture& texture) {
    texturesLoaded.push_back(texture);
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes.at(i).getTextures().push_back(texture);
    }
}


