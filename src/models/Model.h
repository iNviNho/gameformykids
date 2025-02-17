

#ifndef MODEL_H
#define MODEL_H
#include "Mesh.h"
#include "../shaders/shader.h"
#include "assimp/scene.h"
#include "assimp/mesh.h"


class Model
{
public:
    Model(char *modelPath);
    void Draw(Shader &shader);
private:
    // model data
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> texturesLoaded;
    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};



#endif //MODEL_H
