#include "Terrain.h"


#include <glad/glad.h>

#include "EntitiesHolder.h"
#include "Grasses.h"
#include "../images/Image.h"
#include "../models/Model.h"
#include "../models/ModelGenerator.h"
#include "../shaders/shader.h"
#include "../textures/TextureLoader.h"
#include <glm/glm.hpp>
#include <data_dir.h>

#include "../utils/Log.h"

using path = std::filesystem::path;

Terrain::Terrain(const std::filesystem::path& heightMap, const std::filesystem::path& blendMap)
    : heightMap(heightMap), blendMap(blendMap), grasses(EntitiesHolder(std::vector<Entity>())) {
    const GLsizeiptr dataPointsSz = SIZE * SIZE * DATA_PER_LOC;
    const std::unique_ptr<GLfloat[]> dataPoints(new GLfloat[dataPointsSz]);
    generateTextures();
    generateTerrain(dataPoints);
    generateVaoVbo(dataPoints, dataPointsSz);
    generateGrasses();
}

void Terrain::generateTextures() {
    this->grassTexture = TextureLoader::loadTexture(data_dir() /= path("resources/images/green-grass4.png"));
    this->pathTexture = TextureLoader::loadTexture(data_dir() /= path("resources/images/path.png"));
    this->mudTexture = TextureLoader::loadTexture(data_dir() /= path("resources/images/mud.png"));
    this->flowersTexture = TextureLoader::loadTexture(data_dir() /= path("resources/images/grassFlowers.png"));
    this->blendMapTexture = TextureLoader::loadTexture(data_dir() /= path("resources/images/blendMap4.png"));
}


void Terrain::generateVaoVbo(const std::unique_ptr<GLfloat[]>& dataPoints, const GLsizeiptr dataPointsSz) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, dataPointsSz * sizeof(GLfloat), dataPoints.get(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, DATA_PER_GL_VERTEX * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, DATA_PER_GL_VERTEX * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, DATA_PER_GL_VERTEX * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
}

void Terrain::generateGrasses() {
    Log::logInfo("Generating grasses");
    std::shared_ptr<Model> grass = ModelGenerator::generateGrass(data_dir() /= path("resources/objects/grass4/grass.png"));

    std::vector<Entity> entities;
    float blendMapWidthToTerrainSizeRatio = blendMap.getWidth() / SIZE;
    // TODO: All of this could be precomputated during build time instead of
    // doing it at runtime to increase start time
    for (int x = 0; x < SIZE; x++) {
        for (int z = 0; z < SIZE; z++) {
            for (int p = 0; p < GRASS_DENSITY; p++) {
                // random between 0.0f and 0.99f
                float anyX = static_cast<float>(rand()) / (RAND_MAX + 1.0f);
                float xpos = static_cast<float>(x) + anyX;
                // random between 0.0f and 0.99f
                float anyZ = static_cast<float>(rand()) / (RAND_MAX + 1.0f);
                float zpos = -(static_cast<float>(z) + anyZ);
                // temporary increasing y position by 0.35f
                // TODO: long term fix is to use barycentric coordinates
                // to calculate exact height at given x, y position
                // so this method works with any kind of models
                float ypos = getHeight(xpos, zpos) + 0.35f;

                int xPosRatio = static_cast<int>(xpos * blendMapWidthToTerrainSizeRatio);
                int zPosRatio =
                    // we do this because image is flipped vertically
                    blendMap.getHeight() - 1 -
                    static_cast<int>(zpos * blendMapWidthToTerrainSizeRatio * -1);
                if (!blendMap.isBlackColor(xPosRatio, zPosRatio)) {
                    continue;
                }

                entities.emplace_back(grass, glm::vec3(xpos, ypos, zpos));
            }
        }
    }

    Log::logInfo("Generated grasses: " + std::to_string(entities.size()));
    this->grasses = Grasses{EntitiesHolder{std::move(entities)}};
}

const float Terrain::getHeight(float x, float z) const {
    // negate z because opengl is righthanded system
    // and z is negative when we generate terrain in from of us
    z = -z;
    if (x < 0 || z < 0 || x >= SIZE || z >= SIZE) {
        return 0;
    }

    // gives value from 0 to 255
    float grayscaleValue = heightMap.getGrayscaleValue(x, z);
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

float barryCentric(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec2& pos) {
    float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
    if (det == 0.0f) {
        // Handle the degenerate case (e.g., return a default value or throw an error)
        return 0.0f; // Default value
    }
    float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
    float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
    float l3 = 1.0f - l1 - l2;
    return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

const float Terrain::GetHeightOfTerrain(float playerPositionX, float playerPositionZ) const {
    // we are going to find out in which square we are
    // and then we are going to find out in which triangle we are
    // and then we are going to calculate height of terrain
    // based on that triangle
    int playerPositionXInt = static_cast<int>(playerPositionX);
    int playerPositionZInt = static_cast<int>(playerPositionZ);
    // now we need to recreate 4 vertices a,b,c,d the same way like we created them
    // in generateTerrain() method
    auto a = glm::vec3(playerPositionXInt, getHeight(playerPositionXInt, playerPositionZInt), playerPositionZInt);
    auto b = glm::vec3(playerPositionXInt + 1.0f, getHeight(playerPositionXInt + 1.0f, playerPositionZInt), playerPositionZInt);
    auto c = glm::vec3(playerPositionXInt + 1.0f, getHeight(playerPositionXInt + 1.0f, playerPositionZInt - 1.0f), playerPositionZInt - 1.0f);
    auto d = glm::vec3(playerPositionXInt, getHeight(playerPositionXInt, playerPositionZInt - 1.0f), playerPositionZInt - 1.0f);

    float playerPositionXIntNormalized = playerPositionX - playerPositionXInt;
    float playerPositionZIntNormalized = playerPositionZ - playerPositionZInt;

    if (-playerPositionZIntNormalized < playerPositionXIntNormalized) {
        // we are in the first triangle (C,A,B)
        return barryCentric(
            c, a, b,
            glm::vec2(playerPositionX, playerPositionZ)
        );
    }
    // we are in the second triangle (C,D,A)
    return barryCentric(
        c, d, a,
        glm::vec2(playerPositionX, playerPositionZ)
    );
}

void Terrain::generateTerrain(const std::unique_ptr<GLfloat[]>& dataPoints) {
    int vertexPointer = 0;
    glm::vec3 normal;

    for (int x = 0; x < SIZE; x++) {
        for (int z = 0; z < SIZE; z++) {
            const auto floatX = static_cast<float>(x);
            // negate z because opengl is righthanded system
            // and we want to have terrain in front of us
            const auto floatZ = -static_cast<float>(z);

            // a is bottom left
            glm::vec3 a = glm::vec3(floatX, getHeight(floatX, floatZ), floatZ);
            // b is bottom right
            glm::vec3 b = glm::vec3(floatX + 1.0f, getHeight(floatX + 1.0f, floatZ), floatZ);
            // c is top right
            glm::vec3 c = glm::vec3(floatX + 1.0f, getHeight(floatX + 1.0f, floatZ - 1.0f), floatZ - 1.0f);
            // d is top left
            glm::vec3 d = glm::vec3(floatX, getHeight(floatX, floatZ - 1.0f), floatZ - 1.0f);

            // Let's define FIRST TRIANGLE in COUNTER-CLOCKWISE order
            // C,A,B
            // vertices
            dataPoints[vertexPointer * DATA_PER_LOC] = c.x;
            dataPoints[vertexPointer * DATA_PER_LOC + 1] = c.y;
            dataPoints[vertexPointer * DATA_PER_LOC + 2] = c.z;
            // tex coord
            dataPoints[vertexPointer * DATA_PER_LOC + 3] = 1.0f;
            dataPoints[vertexPointer * DATA_PER_LOC + 4] = 1.0f;
            // normals
            normal = calculateNormal(c.x, c.z);
            dataPoints[vertexPointer * DATA_PER_LOC + 5] = normal.x;
            dataPoints[vertexPointer * DATA_PER_LOC + 6] = normal.y;
            dataPoints[vertexPointer * DATA_PER_LOC + 7] = normal.z;

            // vertices
            dataPoints[vertexPointer * DATA_PER_LOC + 8] = a.x;
            dataPoints[vertexPointer * DATA_PER_LOC + 9] = a.y;
            dataPoints[vertexPointer * DATA_PER_LOC + 10] = a.z;
            // tex coord
            dataPoints[vertexPointer * DATA_PER_LOC + 11] = 0.0f;
            dataPoints[vertexPointer * DATA_PER_LOC + 12] = 0.0f;
            // normals
            normal = calculateNormal(a.x, a.z);
            dataPoints[vertexPointer * DATA_PER_LOC + 13] = normal.x;
            dataPoints[vertexPointer * DATA_PER_LOC + 14] = normal.y;
            dataPoints[vertexPointer * DATA_PER_LOC + 15] = normal.z;

            // vertices
            dataPoints[vertexPointer * DATA_PER_LOC + 16] = b.x;
            dataPoints[vertexPointer * DATA_PER_LOC + 17] = b.y;
            dataPoints[vertexPointer * DATA_PER_LOC + 18] = b.z;
            // tex coord
            dataPoints[vertexPointer * DATA_PER_LOC + 19] = 1.0f;
            dataPoints[vertexPointer * DATA_PER_LOC + 20] = 0.0f;
            // normals
            normal = calculateNormal(b.x, b.z);
            dataPoints[vertexPointer * DATA_PER_LOC + 21] = normal.x;
            dataPoints[vertexPointer * DATA_PER_LOC + 22] = normal.y;
            dataPoints[vertexPointer * DATA_PER_LOC + 23] = normal.z;

            // Let's define SECOND triangle in COUNTER-CLOCKWISE order
            // C,D,A
            // vertices
            dataPoints[vertexPointer * DATA_PER_LOC + 24] = c.x;
            dataPoints[vertexPointer * DATA_PER_LOC + 25] = c.y;
            dataPoints[vertexPointer * DATA_PER_LOC + 26] = c.z;
            // tex coord
            dataPoints[vertexPointer * DATA_PER_LOC + 27] = 1.0f;
            dataPoints[vertexPointer * DATA_PER_LOC + 28] = 1.0f;
            // normals
            normal = calculateNormal(c.x, c.z);
            dataPoints[vertexPointer * DATA_PER_LOC + 29] = normal.x;
            dataPoints[vertexPointer * DATA_PER_LOC + 30] = normal.y;
            dataPoints[vertexPointer * DATA_PER_LOC + 31] = normal.z;

            // vertices
            dataPoints[vertexPointer * DATA_PER_LOC + 32] = d.x;
            dataPoints[vertexPointer * DATA_PER_LOC + 33] = d.y;
            dataPoints[vertexPointer * DATA_PER_LOC + 34] = d.z;
            // tex coord
            dataPoints[vertexPointer * DATA_PER_LOC + 35] = 0.0f;
            dataPoints[vertexPointer * DATA_PER_LOC + 36] = 1.0f;
            // normals
            normal = calculateNormal(d.x, d.z);
            dataPoints[vertexPointer * DATA_PER_LOC + 37] = normal.x;
            dataPoints[vertexPointer * DATA_PER_LOC + 38] = normal.y;
            dataPoints[vertexPointer * DATA_PER_LOC + 39] = normal.z;

            // vertices
            dataPoints[vertexPointer * DATA_PER_LOC + 40] = a.x;
            dataPoints[vertexPointer * DATA_PER_LOC + 41] = a.y;
            dataPoints[vertexPointer * DATA_PER_LOC + 42] = a.z;
            // tex coord
            dataPoints[vertexPointer * DATA_PER_LOC + 43] = 0.0f;
            dataPoints[vertexPointer * DATA_PER_LOC + 44] = 0.0f;
            // normals
            normal = calculateNormal(a.x, a.z);
            dataPoints[vertexPointer * DATA_PER_LOC + 45] = normal.x;
            dataPoints[vertexPointer * DATA_PER_LOC + 46] = normal.y;
            dataPoints[vertexPointer * DATA_PER_LOC + 47] = normal.z;

            vertexPointer++;
        }
    }
}

void Terrain::activateTextures(Shader& shader) {
    shader.use();
    shader.setInt("grass", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->GetGrassTexture());
    shader.setInt("path", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->GetPathTexturre());
    shader.setInt("mud", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, this->GetMudTexture());
    shader.setInt("flowers", 3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, this->GetFlowersTexture());
    shader.setInt("blendMap", 4);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, this->GetBlendMapTexture());
}



const float Terrain::GetCountOfVertices() const {
    return SIZE * SIZE * GL_VERTICES_PER_LOC;
}


