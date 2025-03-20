

#ifndef MODEL_H
#define MODEL_H
#include "Mesh.h"
#include "../shaders/shader.h"
#include "assimp/scene.h"
#include "assimp/mesh.h"


class Model
{
public:
    explicit Model(char *modelPath);
    Model(char *modelPath, char *texturePath);
    Model(char *modelPath, Texture *texture);
    explicit Model(Mesh &mesh);

    void Draw(Shader *shader) const;

    const std::vector<Mesh> &GetMeshes() const { return this->meshes;}
private:
    std::vector<Mesh> meshes;
    std::vector<Texture> texturesLoaded;
    std::string directory;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    void loadSingleTexture(char *texturePath);
    void loadSingleTexture(Texture *texture);
};



#endif //MODEL_H
