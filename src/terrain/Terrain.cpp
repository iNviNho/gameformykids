#include "Terrain.h"
#include "iostream"

#include <glad/glad.h>

#include "../textures/TextureLoader.h"

Terrain::Terrain(const int xPos, const int zPos) {
    this->xPos = xPos * SIZE;
    this->zPos = zPos * SIZE;
    dataPoints = new float[VERTEX_COUNT * VERTEX_COUNT * 30];
    generateTexture();
    generateTerrain();
    generateVaoVbo();
}

void Terrain::generateTexture() {
    this->texture = TextureLoader::loadTexture("/Users/vladino/CLionProjects/mygame/resources/images/green-grass.jpg");
    std::cout << "Texture loaded: " << texture << std::endl;
}


void Terrain::generateVaoVbo() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, GetDataPointsSize(), GetDataPoints(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Terrain::generateTerrain() {
    int vertexPointer = 0;
    int numberOfPointsPerLoop = 2 * 3 * 5;
    for (int x = 0; x < VERTEX_COUNT; x++) {
        for (int z = 0; z < VERTEX_COUNT; z++) {
        // first triangle
            // 330 = 1 * 26
        dataPoints[vertexPointer * numberOfPointsPerLoop] = float(x); // a
        dataPoints[vertexPointer * numberOfPointsPerLoop + 1] = 0.0f; // a
        dataPoints[vertexPointer * numberOfPointsPerLoop + 2] = -float(z); // a
        // tex coord
        dataPoints[vertexPointer * numberOfPointsPerLoop + 3] = 0.0f; // b]
        dataPoints[vertexPointer * numberOfPointsPerLoop + 4] = 1.0f; // b]

        dataPoints[vertexPointer * numberOfPointsPerLoop + 5] = float(x) + 1.0f; // b
        dataPoints[vertexPointer * numberOfPointsPerLoop + 6] = 0.0f;
        dataPoints[vertexPointer * numberOfPointsPerLoop + 7] = -float(z);
        // tex coord
        dataPoints[vertexPointer * numberOfPointsPerLoop + 8] = 0.0f; // b]
        dataPoints[vertexPointer * numberOfPointsPerLoop + 9] = 0.0f; // b]

        dataPoints[vertexPointer * numberOfPointsPerLoop + 10] = float(x) + 1.0f; // c
        dataPoints[vertexPointer * numberOfPointsPerLoop + 11] = 0.0f;
        dataPoints[vertexPointer * numberOfPointsPerLoop + 12] = -float(z) - 1.0f;
        // tex coord
        dataPoints[vertexPointer * numberOfPointsPerLoop + 13] = 1.0f; // b]
        dataPoints[vertexPointer * numberOfPointsPerLoop + 14] = 0.0f; // b]

        // second triangle
        dataPoints[vertexPointer * numberOfPointsPerLoop + 15] = float(x); // a
        dataPoints[vertexPointer * numberOfPointsPerLoop + 16] = 0.0f; // a
        dataPoints[vertexPointer * numberOfPointsPerLoop + 17] = -float(z) - 1.0f; // a
        // tex coord
        dataPoints[vertexPointer * numberOfPointsPerLoop + 18] = 1.0f; // b]
        dataPoints[vertexPointer * numberOfPointsPerLoop + 19] = 1.0f; // b]

        dataPoints[vertexPointer * numberOfPointsPerLoop + 20] = float(x) + 1.0f; // c
        dataPoints[vertexPointer * numberOfPointsPerLoop + 21] = 0.0f;
        dataPoints[vertexPointer * numberOfPointsPerLoop + 22] = -float(z) - 1.0f;
        // tex coord
        dataPoints[vertexPointer * numberOfPointsPerLoop + 23] = 1.0f; // b]
        dataPoints[vertexPointer * numberOfPointsPerLoop + 24] = 0.0f; // b]

        dataPoints[vertexPointer * numberOfPointsPerLoop + 25] = float(x); // d
        dataPoints[vertexPointer * numberOfPointsPerLoop + 26] = 0.0f;
        dataPoints[vertexPointer * numberOfPointsPerLoop + 27] = -float(z);
        // tex coord
        dataPoints[vertexPointer * numberOfPointsPerLoop + 28] = 0.0f; // b]
        dataPoints[vertexPointer * numberOfPointsPerLoop + 29] = 1.0f; // b]

        vertexPointer++;
        }
    }
}

const float* Terrain::GetDataPoints() const {
    return dataPoints;
}

long Terrain::GetDataPointsSize() const {
    return sizeof(float) * VERTEX_COUNT * VERTEX_COUNT * 30;
}

const float Terrain::GetCountOfVertices() const {
    return VERTEX_COUNT * VERTEX_COUNT * 6;
}


