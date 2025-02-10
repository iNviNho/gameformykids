#include "Terrain.h"


#include <glad/glad.h>

#include "../images/Image.h"
#include "../shaders/shader.h"
#include "../textures/TextureLoader.h"

Terrain::Terrain(const int xPos, const int zPos, char const* heightMap) {
    this->xPos = xPos * SIZE;
    this->zPos = zPos * SIZE;
    dataPoints = new float[SIZE * SIZE * 30];
    parseHeightMap(heightMap);
    generateTextures();
    generateTerrain();
    generateVaoVbo();
}

void Terrain::generateTextures() {
    this->grassTexture = TextureLoader::loadTexture("/Users/vladino/CLionProjects/mygame/resources/images/green-grass4.png");
    this->pathTexture = TextureLoader::loadTexture("/Users/vladino/CLionProjects/mygame/resources/images/path.png");
    this->mudTexture = TextureLoader::loadTexture("/Users/vladino/CLionProjects/mygame/resources/images/mud.png");
    this->flowersTexture = TextureLoader::loadTexture("/Users/vladino/CLionProjects/mygame/resources/images/grassFlowers.png");
    this->blendMapTexture = TextureLoader::loadTexture("/Users/vladino/CLionProjects/mygame/resources/images/blendMap.png");
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

float Terrain::getHeight(float x, float z) {
    // TODO: why -z ???
    z = -z;
    if (x < 0 || z < 0 || x >= SIZE || z >= SIZE) {
        return 0;
    }

    // gives value from 0 to 255
    float grayscaleValue = heightMap->getGrayscaleValue(x, z);
    // range from 0 to 1
    grayscaleValue /= 255.0f;
    // range from -0.5 to 0.5
    grayscaleValue -= 0.5f;
    // range from -MAX_HEIGHT/2.0f to MAX_HEIGHT/2.0f
    grayscaleValue *= MAX_HEIGHT;
    return grayscaleValue;
}

void Terrain::parseHeightMap(char const* heightMap) {
    this->heightMap = new Image(heightMap);
}

void Terrain::generateTerrain() {
    int vertexPointer = 0;
    int numberOfPointsPerLoop = 2 * 3 * 5;

    for (int x = 0; x < SIZE; x++) {
        for (int z = 0; z < SIZE; z++) {
            const auto floatX = static_cast<float>(x);
            const auto floatZ = static_cast<float>(z);
            dataPoints[vertexPointer * numberOfPointsPerLoop] = floatX; // a
            dataPoints[vertexPointer * numberOfPointsPerLoop + 1] = getHeight(floatX, -floatZ); // a
            dataPoints[vertexPointer * numberOfPointsPerLoop + 2] = -floatZ; // a
            // tex coord
            dataPoints[vertexPointer * numberOfPointsPerLoop + 3] = 0.0f; // b]
            dataPoints[vertexPointer * numberOfPointsPerLoop + 4] = 1.0f; // b]

            dataPoints[vertexPointer * numberOfPointsPerLoop + 5] = floatX + 1.0f; // b
            dataPoints[vertexPointer * numberOfPointsPerLoop + 6] = getHeight(floatX + 1.0f, -floatZ); // b
            dataPoints[vertexPointer * numberOfPointsPerLoop + 7] = -floatZ;
            // tex coord
            dataPoints[vertexPointer * numberOfPointsPerLoop + 8] = 0.0f; // b]
            dataPoints[vertexPointer * numberOfPointsPerLoop + 9] = 0.0f; // b]

            dataPoints[vertexPointer * numberOfPointsPerLoop + 10] = floatX + 1.0f; // c
            dataPoints[vertexPointer * numberOfPointsPerLoop + 11] = getHeight(floatX + 1.0f, -floatZ - 1.0f);
            dataPoints[vertexPointer * numberOfPointsPerLoop + 12] = -floatZ - 1.0f;
            // tex coord
            dataPoints[vertexPointer * numberOfPointsPerLoop + 13] = 1.0f; // b]
            dataPoints[vertexPointer * numberOfPointsPerLoop + 14] = 0.0f; // b]

            // second triangle
            dataPoints[vertexPointer * numberOfPointsPerLoop + 15] = floatX; // a
            dataPoints[vertexPointer * numberOfPointsPerLoop + 16] = getHeight(x, -floatZ - 1.0f); // a
            dataPoints[vertexPointer * numberOfPointsPerLoop + 17] = -floatZ - 1.0f; // a
            // tex coord
            dataPoints[vertexPointer * numberOfPointsPerLoop + 18] = 1.0f; // b]
            dataPoints[vertexPointer * numberOfPointsPerLoop + 19] = 1.0f; // b]

            dataPoints[vertexPointer * numberOfPointsPerLoop + 20] = floatX + 1.0f; // c
            dataPoints[vertexPointer * numberOfPointsPerLoop + 21] = getHeight(floatX + 1.0f, -floatZ - 1.0f);
            dataPoints[vertexPointer * numberOfPointsPerLoop + 22] = -floatZ - 1.0f;
            // tex coord
            dataPoints[vertexPointer * numberOfPointsPerLoop + 23] = 1.0f; // b]
            dataPoints[vertexPointer * numberOfPointsPerLoop + 24] = 0.0f; // b]

            dataPoints[vertexPointer * numberOfPointsPerLoop + 25] = floatX; // d
            dataPoints[vertexPointer * numberOfPointsPerLoop + 26] = getHeight(floatX, -floatZ);
            dataPoints[vertexPointer * numberOfPointsPerLoop + 27] = -floatZ;
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
    return sizeof(float) * SIZE * SIZE * 30;
}

void Terrain::activateTextures(Shader* shader) {
    shader->use();
    shader->setInt("grass", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->GetGrassTexture());
    shader->setInt("path", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->GetPathTexturre());
    shader->setInt("mud", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, this->GetMudTexture());
    shader->setInt("flowers", 3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, this->GetFlowersTexture());
    shader->setInt("blendMap", 4);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, this->GetBlendMapTexture());
}

const float Terrain::GetCountOfVertices() const {
    return SIZE * SIZE * 6;
}


