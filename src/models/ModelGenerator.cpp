

#include "ModelGenerator.h"

#include "Model.h"
#include "../textures/TextureLoader.h"

std::shared_ptr<Model> ModelGenerator::generateCube(const std::filesystem::path& texturePath) {
 // Vertices for a cube
 float vertices[] = {
  // positions          // normals           // texture coords
  -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
   0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
   0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,

  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
   0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,

  -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
  -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
  -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
  -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
   0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
   0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
   0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,

  -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
   0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
   0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
  -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f
};

 // Indices for a cube
 unsigned int indices[] = {
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
};
   std::vector<Vertex> vectorVertices;
   for (int i = 0; i < 192; i += 8) {
       Vertex vertex;
       vertex.Position = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
       vertex.Normal = glm::vec3(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
       vertex.TexCoords = glm::vec2(vertices[i + 6], vertices[i + 7]);
       vectorVertices.push_back(vertex);
   }
    std::vector<unsigned int> vectorIndices;
    for (int i = 0; i < 36; i++) {
     vectorIndices.push_back(indices[i]);
    }
    std::vector<Texture> vectorTextures;
    Texture texture;
    texture.id = TextureLoader::loadTexture(texturePath);
    texture.type = "texture_diffuse";
    texture.path = texturePath;
    vectorTextures.push_back(texture);

   Mesh mesh(vectorVertices, vectorIndices, vectorTextures);

   return std::make_shared<Model>(mesh);
}

std::shared_ptr<Model> ModelGenerator::generateGrass(const std::filesystem::path& texturePath) {
 float vertices[] = {
  // positions          // normals           // texture coords
  -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
   0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
   0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
};

 // Indices for a cube
 unsigned int indices[] = {
   // back face
   0, 1, 2,
   2, 3, 0
 };

 std::vector<Vertex> vectorVertices;
 for (int i = 0; i < 8 * 4; i += 8) {
  Vertex vertex;
  vertex.Position = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
  vertex.Normal = glm::vec3(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
  vertex.TexCoords = glm::vec2(vertices[i + 6], vertices[i + 7]);
  vectorVertices.push_back(vertex);
 }
 std::vector<unsigned int> vectorIndices;
 for (int i = 0; i < 6; i++) {
  vectorIndices.push_back(indices[i]);
 }

 std::vector<Texture> vectorTextures;
 Texture texture;
 texture.id = TextureLoader::loadTexture(texturePath);
 texture.type = "texture_diffuse";
 texture.path = texturePath;
 vectorTextures.push_back(texture);

 Mesh mesh(vectorVertices, vectorIndices, vectorTextures);

 return std::make_shared<Model>(mesh);
}
