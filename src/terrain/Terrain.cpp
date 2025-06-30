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
    auto b = glm::vec3(playerPositionXInt + 1.0f, getHeight(playerPositionXInt + 1, playerPositionZInt), playerPositionZInt);
    auto c = glm::vec3(playerPositionXInt + 1.0f, getHeight(playerPositionXInt + 1, playerPositionZInt - 1), playerPositionZInt - 1.0f);
    auto d = glm::vec3(playerPositionXInt, getHeight(playerPositionXInt, playerPositionZInt - 1), playerPositionZInt - 1.0f);

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

void Terrain::setVertexData(const std::unique_ptr<GLfloat[]>& dataPoints) {
    constexpr const int x = 0;
    constexpr const int z = 0;

    // data start for location (x, z)
    GLfloat* const dataPtrStart = dataPoints.get() + (z * SIZE * DATA_PER_LOC + x * DATA_PER_LOC);
    GLfloat* dataPtr = dataPtrStart;
    
	constexpr const int xPlus1 = x + 1;
	constexpr const int zPlus1 = z + 1;

    constexpr const float floatX = static_cast<float>(x);
    constexpr const float floatXPlus1 = static_cast<float>(xPlus1);

    // negate z because opengl is righthanded system
    // and we want to have terrain in front of us
    constexpr const float floatZ = -static_cast<float>(z);
    constexpr const float floatZPlus1 = -static_cast<float>(zPlus1);

    // a is bottom left
    const glm::vec3 a = glm::vec3(floatX, getHeight(x, -z), floatZ);
    // b is bottom right
    const glm::vec3 b = glm::vec3(floatXPlus1, getHeight(xPlus1, -z), floatZ);
    // c is top right
    const glm::vec3 c = glm::vec3(floatXPlus1, getHeight(xPlus1, -zPlus1), floatZPlus1);
    // d is top left
    const glm::vec3 d = glm::vec3(floatX, getHeight(x, -zPlus1), floatZPlus1);

    // Let's define FIRST TRIANGLE in COUNTER-CLOCKWISE order
    // C,A,B
    dataPtr = setGLVertexData(dataPtr, c, glm::vec<2, int>{xPlus1, -zPlus1}, 1.0f, 1.0f);
    dataPtr = setGLVertexData(dataPtr, a, glm::vec<2, int>{x, -z}, 0.0f, 0.0f);
    dataPtr = setGLVertexData(dataPtr, b, glm::vec<2, int>{xPlus1, -z}, 1.0f, 0.0f);

    // Let's define SECOND triangle in COUNTER-CLOCKWISE order
    // C,D,A
	dataPtr = std::copy(dataPtrStart, dataPtrStart + DATA_PER_GL_VERTEX, dataPtr);
	dataPtr = setGLVertexData(dataPtr, d, glm::vec<2, int>{x, -zPlus1}, 0.0f, 1.0f);
    dataPtr = std::copy(dataPtrStart + 1 * DATA_PER_GL_VERTEX, dataPtrStart + 2 * DATA_PER_GL_VERTEX, dataPtr);
}

void Terrain::setVertexData_x(const std::unique_ptr<GLfloat[]>& dataPoints, const int x) {
    constexpr const int z = 0;

    // data start for location (x, z)
    GLfloat* const dataPtrStart = dataPoints.get() + (z * SIZE * DATA_PER_LOC + x * DATA_PER_LOC);
    GLfloat* dataPtr = dataPtrStart;
    const GLfloat* const dataPtrXMinus1 = dataPoints.get() + ((x-1) * DATA_PER_LOC);
    
    const int xPlus1 = x + 1;
    constexpr const int zPlus1 = z + 1;

    const float floatXPlus1 = static_cast<float>(xPlus1);

    // b is bottom right
    const glm::vec3 b = glm::vec3(floatXPlus1, getHeight(xPlus1, -z), -static_cast<float>(z));
    // c is top right
    const glm::vec3 c = glm::vec3(floatXPlus1, getHeight(xPlus1, -zPlus1), -static_cast<float>(zPlus1));

    // Let's define FIRST TRIANGLE in COUNTER-CLOCKWISE order
    // C,A,B
    dataPtr = setGLVertexData(dataPtr, c, glm::vec<2, int>{xPlus1, -zPlus1}, 1.0f, 1.0f);
    // vertex 'a' is equal to vertex 'b' of a previously computed location
    dataPtr = copyGLVertexData(dataPtr, dataPtrXMinus1 + 2 * DATA_PER_GL_VERTEX, 0.0f, 0.0f);
    dataPtr = setGLVertexData(dataPtr, b, glm::vec<2, int>{xPlus1, -z}, 1.0f, 0.0f);

    // Let's define SECOND triangle in COUNTER-CLOCKWISE order
    // C,D,A
    dataPtr = std::copy(dataPtrStart, dataPtrStart + DATA_PER_GL_VERTEX, dataPtr);
	// vertex 'd' is equal to vertex 'c' of a previously computed location
    dataPtr = copyGLVertexData(dataPtr, dataPtrXMinus1 + 0 * DATA_PER_GL_VERTEX, 0.0f, 1.0f);
    dataPtr = std::copy(dataPtrStart + 1 * DATA_PER_GL_VERTEX, dataPtrStart + 2 * DATA_PER_GL_VERTEX, dataPtr);
}

void Terrain::setVertexData_z(const std::unique_ptr<GLfloat[]>& dataPoints, const int z) {
    constexpr const int x = 0;

    // data start for location (x, z)
    GLfloat* const dataPtrStart = dataPoints.get() + (z * SIZE * DATA_PER_LOC + x * DATA_PER_LOC);
    GLfloat* dataPtr = dataPtrStart;
    const GLfloat* const dataPtrZMinus1 = dataPoints.get() + ((z-1) * SIZE * DATA_PER_LOC);
	
    constexpr const int xPlus1 = 1;
    const int zPlus1 = z + 1;

    // negate z because opengl is righthanded system
    // and we want to have terrain in front of us
    const float floatZPlus1 = -static_cast<float>(zPlus1);

    // c is top right
    const glm::vec3 c = glm::vec3(static_cast<float>(xPlus1), getHeight(xPlus1, -zPlus1), floatZPlus1);
    // d is top left
    const glm::vec3 d = glm::vec3(static_cast<float>(x), getHeight(x, -zPlus1), floatZPlus1);

    // Let's define FIRST TRIANGLE in COUNTER-CLOCKWISE order
    // C,A,B
    dataPtr = setGLVertexData(dataPtr, c, glm::vec<2, int>{xPlus1, -zPlus1}, 1.0f, 1.0f);
	// vertex 'a' is equal to vertex 'd' of a previously computed location
    dataPtr = copyGLVertexData(dataPtr, dataPtrZMinus1 + 4 * DATA_PER_GL_VERTEX, 0.0f, 0.0f);
    // vertex 'b' is equal to vertex 'c' of a previously computed location
    dataPtr = copyGLVertexData(dataPtr, dataPtrZMinus1 + 0 * DATA_PER_GL_VERTEX, 1.0f, 0.0f);

    // Let's define SECOND triangle in COUNTER-CLOCKWISE order
    // C,D,A
    dataPtr = std::copy(dataPtrStart, dataPtrStart + DATA_PER_GL_VERTEX, dataPtr);
	dataPtr = setGLVertexData(dataPtr, d, glm::vec<2, int>{x, -zPlus1}, 0.0f, 1.0f);
    dataPtr = std::copy(dataPtrStart + 1 * DATA_PER_GL_VERTEX, dataPtrStart + 2 * DATA_PER_GL_VERTEX, dataPtr);
}

void Terrain::setVertexData_xz(const std::unique_ptr<GLfloat[]>& dataPoints, const int x, const int z) {
    // data start for location (x, z)
    GLfloat* const dataPtrStart = dataPoints.get() + (z * SIZE * DATA_PER_LOC + x * DATA_PER_LOC);
    GLfloat* dataPtr = dataPtrStart;
    const GLfloat* const dataPtrXMinus1 = dataPoints.get() + (z * SIZE * DATA_PER_LOC + (x-1) * DATA_PER_LOC);
    const GLfloat* const dataPtrZMinus1 = dataPoints.get() + ((z-1) * SIZE * DATA_PER_LOC + x * DATA_PER_LOC);

    const int xPlus1 = x + 1;
    const int zPlus1 = z + 1;

    const float floatXPlus1 = static_cast<float>(xPlus1);

    // negate z because opengl is righthanded system
    // and we want to have terrain in front of us
    const float floatZPlus1 = -static_cast<float>(zPlus1);

    // c is top right
    const glm::vec3 c = glm::vec3(floatXPlus1, getHeight(xPlus1, -zPlus1), floatZPlus1);

    // Let's define FIRST TRIANGLE in COUNTER-CLOCKWISE order
    // C,A,B
    dataPtr = setGLVertexData(dataPtr, c, glm::vec<2, int>{xPlus1, -zPlus1}, 1.0f, 1.0f);
    // vertex 'a' is equal to vertex 'b' of a previously computed location
    dataPtr = copyGLVertexData(dataPtr, dataPtrXMinus1 + 2 * DATA_PER_GL_VERTEX, 0.0f, 0.0f);
    // vertex 'b' is equal to vertex 'c' of a previously computed location
    dataPtr = copyGLVertexData(dataPtr, dataPtrZMinus1 + 0 * DATA_PER_GL_VERTEX, 1.0f, 0.0f);

    // Let's define SECOND triangle in COUNTER-CLOCKWISE order
    // C,D,A
    dataPtr = std::copy(dataPtrStart, dataPtrStart + DATA_PER_GL_VERTEX, dataPtr);
    // vertex 'd' is equal to vertex 'c' of a previously computed location
    dataPtr = copyGLVertexData(dataPtr, dataPtrXMinus1 + 0 * DATA_PER_GL_VERTEX, 0.0f, 1.0f);
    dataPtr = std::copy(dataPtrStart + 1 * DATA_PER_GL_VERTEX, dataPtrStart + 2 * DATA_PER_GL_VERTEX, dataPtr);
}

void Terrain::generateTerrain(const std::unique_ptr<GLfloat[]>& dataPoints) {
    setVertexData(dataPoints);

    for(int x = 1; x < SIZE; ++x)
        setVertexData_x(dataPoints, x);

    for (int z = 1; z < SIZE; ++z)
        setVertexData_z(dataPoints, z);

    for (int z = 1; z < SIZE; z++)
        for (int x = 1; x < SIZE; x++)
            setVertexData_xz(dataPoints, x, z);
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


