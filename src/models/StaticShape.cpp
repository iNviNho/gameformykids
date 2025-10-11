#include "StaticShape.h"

void StaticShape::generate(const std::string& texturePath) {
    // prepare vertices
    // generate 6 vertices for 2 triangles with correct texture coords
    // Each vertex: x, y, z, u, v
    float vertices[] = {
        // First triangle
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // bottom left
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // bottom right
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // top right
        // Second triangle
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // bottom left
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // top right
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f  // top left
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    texture = TextureLoader::loadTexture(texturePath);
}