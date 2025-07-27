

#ifndef MODEL_H
#define MODEL_H
#include <filesystem>
#include "Mesh.h"
#include "../shaders/shader.h"
#include "assimp/scene.h"
#include "assimp/mesh.h"


class Model
{
public:
    explicit Model(const std::filesystem::path& modelPath);
    Model(const std::filesystem::path& modelPath, const std::filesystem::path& texturePath);
    Model(const std::filesystem::path& modelPath, Texture& texture);
    explicit Model(Mesh&& mesh);

    void Draw(Shader& shader) const;

    const std::vector<Mesh> &GetMeshes() const { return this->meshes;}
private:
    std::vector<Mesh> meshes;
    std::vector<Texture> texturesLoaded;
    std::filesystem::path directory;

    void loadModel(const std::filesystem::path& path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    void loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string& typeName, std::vector<Texture>& textures);
    void loadSingleTexture(const std::filesystem::path& texturePath);
    void loadSingleTexture(Texture& texture);
};



#endif //MODEL_H
