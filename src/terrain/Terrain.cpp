#include "Terrain.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <limits>
#include <glad/glad.h>
#include "../objects/Entity.h"
#include "EntitiesHolder.h"
#include "Grasses.h"
#include "../models/AbstractModel.h"
#include "../models/ModelGenerator.h"
#include "../textures/TextureLoader.h"
#include <glm/glm.hpp>
#include <data_dir.h>

#include "../tracing/Tracer.h"
#include "../utils/Log.h"

using path = std::filesystem::path;

Terrain::Terrain(const std::filesystem::path& blendMap)
    : blendMap(blendMap), terrainHeight(SIZE) {
    const GLsizeiptr dataPointsSz = SIZE * SIZE * DATA_PER_LOC;
    dataPoints = std::make_unique<GLfloat[]>(dataPointsSz);
    generateTextures();
    generateTerrain(dataPoints);
    generateVaoVbo(dataPoints, dataPointsSz);
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
    BufferTerrainDataPoints();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, DATA_PER_GL_VERTEX * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, DATA_PER_GL_VERTEX * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, DATA_PER_GL_VERTEX * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
}

void Terrain::BufferTerrainDataPoints() {
    TRACE_ME();
    const int dataPointsSize =  SIZE * SIZE * DATA_PER_LOC;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, dataPointsSize * sizeof(GLfloat), dataPoints.get(), GL_STATIC_DRAW);
}

void Terrain::ReloadTerrain(const std::vector<glm::vec3>& newDataPoints) {
    terrainHeight.loadDataFromStorage();
    UpdateVertexData(newDataPoints);
    BufferTerrainDataPoints();
}

void Terrain::UpdateVertexData(const std::vector<glm::vec3>& newDataPoints) {
    std::unordered_set<int> dirty;

    auto index = [](int x, int z) {
        return z * SIZE + x;
    };

    // mark dirty + neighbors
    for (const auto& p : newDataPoints)
    {
        int x = (int)p.x;
        int z = (int)(-p.z);

        for (int dz = 0; dz <= 1; dz++)
        for (int dx = 0; dx <= 1; dx++)
        {
            int nx = x + dx;
            int nz = z + dz;

            if (nx >= 0 && nz >= 0 && nx < SIZE && nz < SIZE)
                dirty.insert(index(nx, nz));
        }
    }

    // rebuild in safe order
    for (int z = 0; z < SIZE; z++)
        for (int x = 0; x < SIZE; x++)
        {
            if (dirty.find(index(x, z)) != dirty.end())
                setVertexData<false,false>(dataPoints, x, z);
        }
}

float Terrain::GetHeight(const int x, int z) const {
    // we negate SIZE becuase OpenGL is right handed system
    // so the more forward we go, the more negative z will be
    if (x < 0 || z > 0 || x >= SIZE || z <= SIZE * -1) {
        return 0;
    }

    return terrainHeight.get(x, z);
}

glm::vec3 Terrain::calculateNormal(const int x, const int z) const {
    // calculate normal using finite difference method
    float heightL = GetHeight(x - 1, z);
    float heightR = GetHeight(x + 1, z);
    float heightD = GetHeight(x, z - 1);
    float heightU = GetHeight(x, z + 1);
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

float Terrain::GetHeight(const float playerPositionX, const float playerPositionZ) const {
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
    const auto a = glm::vec3(playerPositionXInt, GetHeight(playerPositionXInt, playerPositionZInt), playerPositionZInt);
    const auto b = glm::vec3(playerPositionXInt + 1.0f, GetHeight(playerPositionXInt + 1, playerPositionZInt), playerPositionZInt);
    const auto c = glm::vec3(playerPositionXInt + 1.0f, GetHeight(playerPositionXInt + 1, playerPositionZInt - 1), playerPositionZInt - 1.0f);
    const auto d = glm::vec3(playerPositionXInt, GetHeight(playerPositionXInt, playerPositionZInt - 1), playerPositionZInt - 1.0f);

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

float Terrain::GetHeight(const glm::vec4& plane, const float x, const float z)
{
    if (std::fabs(plane.y) < std::numeric_limits<float>::epsilon())
        throw std::runtime_error("Unable to calculate height for a vertical terrain");
    return (-(x * plane.x) - (z * plane.z) - plane.w) / plane.y;
}

glm::vec4 Terrain::GetTrianglePlane(const float x, const float z) const
{
    return GetTrianglePlane(GetTriangle(x, z));
}

bool Terrain::IsInsideTriangle(const std::array<glm::vec3, 3>& triangle, const glm::vec3& n, const glm::vec3& ptInPlane) noexcept
{
    const glm::vec3 v1 = glm::normalize(triangle[1] - triangle[0]);
    const glm::vec3 v2 = glm::normalize(triangle[2] - triangle[1]);
    const glm::vec3 v3 = glm::normalize(triangle[0] - triangle[2]);

    const glm::vec3 ptVec1 = glm::normalize(ptInPlane - triangle[0]);
    const glm::vec3 ptVec2 = glm::normalize(ptInPlane - triangle[1]);
    const glm::vec3 ptVec3 = glm::normalize(ptInPlane - triangle[2]);

    return (glm::dot(glm::cross(v1, ptVec1), n) >= 0) &&
           (glm::dot(glm::cross(v2, ptVec2), n) >= 0) &&
           (glm::dot(glm::cross(v3, ptVec3), n) >= 0);
}

void Terrain::generateTerrain(
        const std::unique_ptr<GLfloat[]>& dataPoints) {
    TRACE_ME();
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


