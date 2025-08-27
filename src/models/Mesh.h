

#ifndef MESH_H
#define MESH_H
#include <filesystem>
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
    Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, std::vector<Texture>&& textures);

    void Draw(Shader& shader) const;
    void activateTextures(Shader& shader) const;
    void bindVAO() const;

    const unsigned int &GetVAO() const {return VAO;}
    std::vector<Texture> &getTextures() {return textures;}
    const std::vector<Vertex> &getVertices() const {return vertices;}
    const std::vector<unsigned int> &GetIndices() const {return indices;}
private:
    unsigned int VAO, VBO, EBO;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    void setupMesh();
};



#endif //MESH_H
