

#ifndef MESH_H
#define MESH_H
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
    std::string path;
};

class Mesh {
    public:
        unsigned int VAO, VBO, EBO;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        void Draw(Shader &shader);
        void activateTextures(Shader &shader);
        void bindVAO();
    private:
        void setupMesh();
};



#endif //MESH_H
