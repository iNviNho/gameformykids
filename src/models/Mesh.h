

#ifndef MESH_H
#define MESH_H
#include <filesystem>

#include "VertexBoneData.h"
#include "../shaders/shader.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::filesystem::path path;
};

class Mesh {
public:
    Mesh(
        std::vector<Vertex>&& vertices,
        std::vector<unsigned int>&& indices,
        std::vector<Texture>&& textures,
        std::vector<VertexBoneData>&& verticesWithBoneData
    );

    void Draw(Shader& shader) const;
    void activateTextures(Shader& shader) const;
    void bindVAO() const;

    const unsigned int &GetVAO() const {return VAO;}
    std::vector<Texture>& getTextures() {return textures;}
    std::vector<Vertex>& getVertices() {return vertices;}
    std::vector<unsigned int>& GetIndices() {return indices;}
    bool HasBones() const {return hasBones;}
private:
    unsigned int VAO, VBO, EBO, VBOB;
    bool hasBones;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    std::vector<VertexBoneData> verticesWithBoneData;

    void setupMesh();
};



#endif //MESH_H
