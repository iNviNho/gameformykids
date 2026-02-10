

#ifndef MODEL_H
#define MODEL_H
#include <filesystem>
#include <map>

#include "Mesh.h"
#include "../shaders/shader.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"

class AbstractModel
{
public:
    AbstractModel() = default;
    virtual ~AbstractModel() = default;

    explicit AbstractModel(const std::filesystem::path& modelPath);
    AbstractModel(const std::filesystem::path& modelPath, const std::filesystem::path& texturePath);
    AbstractModel(const std::filesystem::path& modelPath, Texture& texture);
    explicit AbstractModel(Mesh&& mesh);
    void Draw(Shader& shader) ;
    std::vector<Mesh> &GetMeshes() { return this->meshes;}
    virtual bool IsAnimated() const = 0;

protected:
    std::vector<Mesh> meshes;
    std::vector<Texture> texturesLoaded;
    std::filesystem::path directory;

    std::vector<glm::mat4> transforms;
    Assimp::Importer importer;
    const aiScene* pScene;

    void loadModel(const std::filesystem::path& path);
    void processNode(const aiScene *scene);
    void loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string& typeName, std::vector<Texture>& textures);
    void loadSingleTexture(const std::filesystem::path& texturePath);
    void loadSingleTexture(Texture& texture);
    // virtual because AnimatedModel needs to override it
    virtual Mesh processMesh(aiMesh *mesh, const aiScene *scene);
};



#endif //MODEL_H
