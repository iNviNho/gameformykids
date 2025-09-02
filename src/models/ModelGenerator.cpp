#include "ModelGenerator.h"

#include "Model.h"
#include "../textures/TextureLoader.h"

std::shared_ptr<Model> ModelGenerator::generateCube(const std::filesystem::path &texturePath) {
    // Vertices for a cube
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f
    };
    std::vector<Vertex> vectorVertices;
    vectorVertices.reserve(24);
    for (int i = 0; i < 192; i += 8) {
        vectorVertices.emplace_back(Vertex{
            glm::vec3{vertices[i], vertices[i + 1], vertices[i + 2]},
            glm::vec3{vertices[i + 3], vertices[i + 4], vertices[i + 5]},
            glm::vec2{vertices[i + 6], vertices[i + 7]}
        });
    }

    return std::make_shared<Model>(
        Mesh{
            std::move(vectorVertices),
            // Indices for a cube
            {
                // back face
                0, 1, 2,
                2, 3, 0,
                // front face
                4, 5, 6,
                6, 7, 4,
                // left face
                8, 9, 10,
                10, 11, 8,
                // right face
                12, 13, 14,
                14, 15, 12,
                // bottom face
                16, 17, 18,
                18, 19, 16,
                // top face
                20, 21, 22,
                22, 23, 20
            },
            {
                {
                    TextureLoader::loadTexture(texturePath),
                    "texture_diffuse",
                    texturePath
                }
            }
        });
}

std::shared_ptr<Model> ModelGenerator::generateGrass(const std::filesystem::path &texturePath) {
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
    };

    std::vector<Vertex> vectorVertices;
    vectorVertices.reserve(4);
    for (int i = 0; i < 8 * 4; i += 8) {
        vectorVertices.emplace_back(Vertex{
            glm::vec3{vertices[i], vertices[i + 1], vertices[i + 2]},
            glm::vec3{vertices[i + 3], vertices[i + 4], vertices[i + 5]},
            glm::vec2{vertices[i + 6], vertices[i + 7]}
        });
    }

    return std::make_shared<Model>(
        Mesh{
            std::move(vectorVertices),
            // Indices for a cube
            {
                // back face
                0, 1, 2,
                2, 3, 0
            },
            {
                {
                    TextureLoader::loadTexture(texturePath),
                    "texture_diffuse",
                    texturePath
                }
            }
        });
}
