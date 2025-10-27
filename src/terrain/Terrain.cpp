#include "Terrain.h"
#include <vector>

#include <glad/glad.h>
#include "../objects/Entity.h"
#include "EntitiesHolder.h"
#include "Grasses.h"
#include "../models/Model.h"
#include "../models/ModelGenerator.h"
#include "../textures/TextureLoader.h"
#include <glm/glm.hpp>
#include <data_dir.h>

#include "../utils/Log.h"

using path = std::filesystem::path;

Terrain::Terrain(const std::filesystem::path& heightMap, const std::filesystem::path& blendMap)
    : heightMap(heightMap), blendMap(blendMap), grasses(EntitiesHolder(std::vector<Entity>())) {
    const GLsizeiptr dataPointsSz = SIZE * SIZE * DATA_PER_LOC;
    // We allocate a fixed-size array on the heap so there is no dynamic recalculation
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
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, DATA_PER_GL_VERTEX * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, DATA_PER_GL_VERTEX * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
}

void Terrain::generateGrasses() {
    Log::logInfo("Generating grasses");
    std::shared_ptr<Model> grass = ModelGenerator::generateGrass(data_dir() /= path("resources/objects/grass4/grass.png"));

    // capacity unknown at this point
    std::vector<Entity> entities;
    float blendMapWidthToTerrainSizeRatio = blendMap.getWidth() / SIZE;
    // TODO: All of this could be precomputated during build time instead of
    // doing it at runtime to speed up start time
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

float Terrain::getHeight(const int x, int z) const {
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

glm::vec3 Terrain::calculateNormal(const int x, const int z) const {
    // calculate normal using finite difference method
    float heightL = getHeight(x - 1, z);
    float heightR = getHeight(x + 1, z);
    float heightD = getHeight(x, z - 1);
    float heightU = getHeight(x, z + 1);
    glm::vec3 normal = glm::vec3(heightL - heightR, 2.0f, heightD - heightU);
    return glm::normalize(normal);
}

float barryCentric(const std::array<glm::vec3, 3>& triangle, const glm::vec2& pos) {
    const auto& [p1, p2, p3] = triangle;
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

float Terrain::GetHeightOfTerrain(const float playerPositionX, const float playerPositionZ) const {
    return barryCentric(GetTriangle(playerPositionX, playerPositionZ), glm::vec2{ playerPositionX, playerPositionZ });
}

std::array<glm::vec3, 3> Terrain::GetTriangle(const float x, const float z) const
{
    // we are going to find out in which square we are
    // and then we are going to find out in which triangle we are
    // and then we are going to calculate height of terrain
    // based on that triangle
    const int playerPositionXInt = static_cast<int>(x);
    const int playerPositionZInt = static_cast<int>(z);
    // now we need to recreate 4 vertices a,b,c,d the same way like we created them
    // in generateTerrain() method
    const auto a = glm::vec3(playerPositionXInt, getHeight(playerPositionXInt, playerPositionZInt), playerPositionZInt);
    const auto b = glm::vec3(playerPositionXInt + 1.0f, getHeight(playerPositionXInt + 1, playerPositionZInt), playerPositionZInt);
    const auto c = glm::vec3(playerPositionXInt + 1.0f, getHeight(playerPositionXInt + 1, playerPositionZInt - 1), playerPositionZInt - 1.0f);
    const auto d = glm::vec3(playerPositionXInt, getHeight(playerPositionXInt, playerPositionZInt - 1), playerPositionZInt - 1.0f);

    const float playerPositionXIntNormalized = x - playerPositionXInt;
    const float playerPositionZIntNormalized = z - playerPositionZInt;

    if (-playerPositionZIntNormalized < playerPositionXIntNormalized)
        return { c, a, b };
    else
        return { c, d, a };
}

glm::vec4 Terrain::GetTrianglePlane(const std::array<glm::vec3, 3>& triangle) noexcept
{
    const auto& [p1, p2, p3] = triangle;

    const glm::vec3 n = glm::normalize(glm::cross(p1 - p3, p2 - p3));
    const float d = -glm::dot(n, p1);
    return { n, d };
}

glm::vec4 Terrain::GetTrianglePlane(const float x, const float z) const
{
    return GetTrianglePlane(GetTriangle(x, z));
}

void Terrain::generateTerrain(const std::unique_ptr<GLfloat[]>& dataPoints) {

    // generate data for location (0,0)
    setVertexData<true, true>(dataPoints, 0, 0);

    // generate data for first row (z=0)
    for (int x = 1; x < SIZE; ++x)
        setVertexData<false, true>(dataPoints, x, 0);

    // generate data for first column (x=0)
    for (int z = 1; z < SIZE; ++z)
        setVertexData<true, false>(dataPoints, 0, z);

    // generate data for the rest of the terrain
    for (int z = 1; z < SIZE; z++)
        for (int x = 1; x < SIZE; x++) {
            setVertexData<false, false>(dataPoints, x, z);
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


