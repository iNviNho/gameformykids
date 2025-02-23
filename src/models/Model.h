

#ifndef MODEL_H
#define MODEL_H
#include "Mesh.h"
#include "../shaders/shader.h"
#include "assimp/scene.h"
#include "assimp/mesh.h"


class Model
{
public:
    Model(glm::vec3 position, char *modelPath);
    Model(glm::vec3 position, char *modelPath, char *texturePath);
    Model(glm::vec3 position, char *modelPath, Texture *texture);
    Model(glm::vec3 position, Mesh &mesh);
    void Draw(Shader &shader);
    void SetPosition(glm::vec3 position) {
        this->position = position;
    }
    glm::vec3 GetPosition() {
        return this->position;
    }
    std::vector<Mesh> GetMeshes() {
        return this->meshes;
    }
private:
    // model data
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> texturesLoaded;
    // TODO: Abstract this to Object class that holds Model
    glm::vec3 position;
    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    void loadSingleTexture(char *texturePath);
    void loadSingleTexture(Texture *texture);
};



#endif //MODEL_H
