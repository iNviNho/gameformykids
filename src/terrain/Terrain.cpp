#include "Terrain.h"


#include <glad/glad.h>

#include "EntitiesHolder.h"
#include "Grasses.h"
#include "../images/Image.h"
#include "../models/Model.h"
#include "../models/ModelGenerator.h"
#include "../shaders/shader.h"
#include "../textures/TextureLoader.h"

Terrain::Terrain(char const* heightMap, char const* blendMap)
    : grasses(EntitiesHolder(std::vector<Entity>())) {
    std::cout << "here" << std::endl;
    dataPoints = new float[SIZE * SIZE * 30];
    parseHeightMap(heightMap);
    parseBlendMap(blendMap);
    generateTextures();
    generateTerrain();
    generateVaoVbo();
    generateGrasses();
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void Terrain::generateGrasses() {
    std::cout << "Generating grasses" << std::endl;
    Model* grass = ModelGenerator::generateGrass("/Users/vladino/CLionProjects/mygame/resources/objects/grass4/grass.png");

    float density = 0.1;
    int perTileEntities = 10 * density;
    // random float between 0 and 1

    // ❤️❤️❤️❤️❤️❤️❤️
    std::vector<Entity> entities;
    for (int x = 0; x < SIZE; x++) {
        for (int z = 0; z < SIZE; z++) {
            for (int p = 0; p < perTileEntities; p++) {
                // random -1 or 1
                float anyX = static_cast<float>(rand() % 100) / 100.0f;
                float xpos = x + anyX;
                float anyZ = static_cast<float>(rand() % 100) / 100.0f;
                float zpos = -(z + anyZ);
                float ypos = getHeight(xpos, zpos) + 0.35f;

                // 1024 / 64 = 16
                // so image is 16x bigger than terrain
                int imageRatio = blendMap->getWidth() / SIZE;
                int xPosRatio = xpos * imageRatio;
                int zPosRatio = zpos * imageRatio;
                if (!blendMap->isBlackColor(xPosRatio, -zPosRatio)) {
                    continue;
                }

                glm::vec3 position = glm::vec3(xpos, ypos, zpos);

                Entity entity(
                    *grass,
                    position
                );
                entities.push_back(entity);
            }
        }
    }

    this->grasses = Grasses(EntitiesHolder(entities));
    std::cout << "Grasses generated: ";
    std::cout << entities.size() << std::endl;
}

float Terrain::getHeight(float x, float z) {
    // negate z because opengl is righthanded system
    // and z is negative when we generate terrain in from of us
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

glm::vec3 Terrain::calculateNormal(float x, float z) {
    // calculate normal using finite difference method
    float heightL = getHeight(x - 1, z);
    float heightR = getHeight(x + 1, z);
    float heightD = getHeight(x, z - 1);
    float heightU = getHeight(x, z + 1);
    glm::vec3 normal = glm::vec3(heightL - heightR, 2.0f, heightD - heightU);
    return glm::normalize(normal);
}

void Terrain::parseHeightMap(char const* heightMap) {
    this->heightMap = new Image(heightMap);
}

void Terrain::parseBlendMap(char const *blendMap) {
    this->blendMap = new Image(blendMap);
}

void Terrain::generateTerrain() {
    int vertexPointer = 0;
    int numberOfPointsPerLoop = 2 * 3 * 8;
    glm::vec3 normal;

    for (int x = 0; x < SIZE; x++) {
        for (int z = 0; z < SIZE; z++) {
            const auto floatX = static_cast<float>(x);
            // negate z because opengl is righthanded system
            // and we want to have terrain in front of us
            const auto floatZ = -static_cast<float>(z);

            dataPoints[vertexPointer * numberOfPointsPerLoop] = floatX; // a
            dataPoints[vertexPointer * numberOfPointsPerLoop + 1] = getHeight(floatX, floatZ); // a
            dataPoints[vertexPointer * numberOfPointsPerLoop + 2] = floatZ; // a
            // tex coord
            dataPoints[vertexPointer * numberOfPointsPerLoop + 3] = 0.0f; // b]
            dataPoints[vertexPointer * numberOfPointsPerLoop + 4] = 1.0f; // b]
            // normals
            normal = calculateNormal(floatX, floatZ);
            dataPoints[vertexPointer * numberOfPointsPerLoop + 5] = normal.x;
            dataPoints[vertexPointer * numberOfPointsPerLoop + 6] = normal.y;
            dataPoints[vertexPointer * numberOfPointsPerLoop + 7] = normal.z;

            dataPoints[vertexPointer * numberOfPointsPerLoop + 8] = floatX + 1.0f; // b
            dataPoints[vertexPointer * numberOfPointsPerLoop + 9] = getHeight(floatX + 1.0f, floatZ); // b
            dataPoints[vertexPointer * numberOfPointsPerLoop + 10] = floatZ;
            // tex coord
            dataPoints[vertexPointer * numberOfPointsPerLoop + 11] = 0.0f; // b]
            dataPoints[vertexPointer * numberOfPointsPerLoop + 12] = 0.0f; // b]
            // normals
            normal = calculateNormal(floatX + 1.0f, floatZ);
            dataPoints[vertexPointer * numberOfPointsPerLoop + 13] = normal.x;
            dataPoints[vertexPointer * numberOfPointsPerLoop + 14] = normal.y;
            dataPoints[vertexPointer * numberOfPointsPerLoop + 15] = normal.z;

            dataPoints[vertexPointer * numberOfPointsPerLoop + 16] = floatX + 1.0f; // c
            dataPoints[vertexPointer * numberOfPointsPerLoop + 17] = getHeight(floatX + 1.0f, floatZ - 1.0f);
            dataPoints[vertexPointer * numberOfPointsPerLoop + 18] = floatZ - 1.0f;
            // tex coord
            dataPoints[vertexPointer * numberOfPointsPerLoop + 19] = 1.0f; // b]
            dataPoints[vertexPointer * numberOfPointsPerLoop + 20] = 0.0f; // b]
            // normals
            normal = calculateNormal(floatX + 1.0f, floatZ - 1.0f);
            dataPoints[vertexPointer * numberOfPointsPerLoop + 21] = normal.x;
            dataPoints[vertexPointer * numberOfPointsPerLoop + 22] = normal.y;
            dataPoints[vertexPointer * numberOfPointsPerLoop + 23] = normal.z;

            glm::vec3 a = glm::vec3(floatX, getHeight(floatX, floatZ), floatZ);
            glm::vec3 b = glm::vec3(floatX + 1.0f, getHeight(floatX + 1.0f, floatZ), floatZ);
            glm::vec3 c = glm::vec3(floatX + 1.0f, getHeight(floatX + 1.0f, floatZ - 1.0f), floatZ - 1.0f);

            // second triangle
            dataPoints[vertexPointer * numberOfPointsPerLoop + 24] = floatX; // a
            dataPoints[vertexPointer * numberOfPointsPerLoop + 25] = getHeight(floatX, floatZ - 1.0f); // a
            dataPoints[vertexPointer * numberOfPointsPerLoop + 26] = floatZ - 1.0f; // a
            // tex coord
            dataPoints[vertexPointer * numberOfPointsPerLoop + 27] = 1.0f; // b]
            dataPoints[vertexPointer * numberOfPointsPerLoop + 28] = 1.0f; // b]
            // normals
            normal = calculateNormal(floatX, floatZ - 1.0f);
            dataPoints[vertexPointer * numberOfPointsPerLoop + 29] = normal.x;
            dataPoints[vertexPointer * numberOfPointsPerLoop + 30] = normal.y;
            dataPoints[vertexPointer * numberOfPointsPerLoop + 31] = normal.z;

            dataPoints[vertexPointer * numberOfPointsPerLoop + 32] = floatX + 1.0f; // c
            dataPoints[vertexPointer * numberOfPointsPerLoop + 33] = getHeight(floatX + 1.0f, floatZ - 1.0f);
            dataPoints[vertexPointer * numberOfPointsPerLoop + 34] = floatZ - 1.0f;
            // tex coord
            dataPoints[vertexPointer * numberOfPointsPerLoop + 35] = 1.0f; // b]
            dataPoints[vertexPointer * numberOfPointsPerLoop + 36] = 0.0f; // b]
            // normals
            normal = calculateNormal(floatX + 1.0f, floatZ - 1.0f);
            dataPoints[vertexPointer * numberOfPointsPerLoop + 37] = normal.x;
            dataPoints[vertexPointer * numberOfPointsPerLoop + 38] = normal.y;
            dataPoints[vertexPointer * numberOfPointsPerLoop + 39] = normal.z;

            dataPoints[vertexPointer * numberOfPointsPerLoop + 40] = floatX; // d
            dataPoints[vertexPointer * numberOfPointsPerLoop + 41] = getHeight(floatX, floatZ);
            dataPoints[vertexPointer * numberOfPointsPerLoop + 42] = floatZ;
            // tex coord
            dataPoints[vertexPointer * numberOfPointsPerLoop + 43] = 0.0f; // b]
            dataPoints[vertexPointer * numberOfPointsPerLoop + 44] = 1.0f; // b]
            // normals
            normal = calculateNormal(floatX, floatZ);
            dataPoints[vertexPointer * numberOfPointsPerLoop + 45] = normal.x;
            dataPoints[vertexPointer * numberOfPointsPerLoop + 46] = normal.y;
            dataPoints[vertexPointer * numberOfPointsPerLoop + 47] = normal.z;

            vertexPointer++;
        }
    }
}



const float* Terrain::GetDataPoints() const {
    return dataPoints;
}

long Terrain::GetDataPointsSize() const {
    return sizeof(float) * SIZE * SIZE * 48;
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


