#ifndef TERRAIN_H
#define TERRAIN_H
#include <memory>
#include <filesystem>
#include "Grasses.h"
#include "../images/Image.h"
#include "../models/Model.h"
#include "../shaders/shader.h"


class Terrain
{
private:

    /**
     * Size of the terrain in vertices.
     * Must be one of 128, 256, 512, 1024 ...
     */
    static constexpr int SIZE = 256;

    /**
     * Defines minimum and maximum height of the terrain.
     */
    static constexpr float MAX_HEIGHT = 30.0f;

    /**
     * Defines how much terrain is covered with grass.
     * e.g. 1 means 10%
     */
    static constexpr int GRASS_DENSITY = 1;

    /**
     * Terrain is modeled with a triangular mesh (2 triangles, 3 vertices per triangle)
     */
    static constexpr int GL_VERTICES_PER_LOC = 2 * 3;

    /**
     * 8 data per vertex (x,y,z,u,v,nx,ny,nz)
     */
    static constexpr int DATA_PER_GL_VERTEX = 8;

    /**
     * GL data per location
     */
    static constexpr int DATA_PER_LOC = DATA_PER_GL_VERTEX * GL_VERTICES_PER_LOC;

    unsigned int VAO, VBO;
    unsigned int grassTexture;
    unsigned int pathTexture;
    unsigned int mudTexture;
    unsigned int flowersTexture;
    unsigned int blendMapTexture;
    Image heightMap;
    Image blendMap;

    void generateTextures();
    void generateVaoVbo(const std::unique_ptr<GLfloat[]>& dataPoints, const GLsizeiptr dataPointsSz);

    void generateTerrain(const std::unique_ptr<GLfloat[]>& dataPoints);
    const float getHeight(float x, float z) const;
    glm::vec3 calculateNormal(float x, float z);

    Grasses grasses;
    void generateGrasses();

public:
    Terrain(const std::filesystem::path& heightMap, const std::filesystem::path& blendMap);
    ~Terrain() = default;
    [[nodiscard]] const float GetCountOfVertices() const;
    [[nodiscard]] unsigned int GetVAO() const { return VAO; }
    [[nodiscard]] unsigned int GetGrassTexture() const { return grassTexture; }
    [[nodiscard]] unsigned int GetPathTexturre() const { return pathTexture; }
    [[nodiscard]] unsigned int GetMudTexture() const { return mudTexture; }
    [[nodiscard]] unsigned int GetFlowersTexture() const { return flowersTexture; }
    [[nodiscard]] unsigned int GetBlendMapTexture() const { return blendMapTexture; }
    [[nodiscard]] int GetSize() { return SIZE; }
    [[nodiscard]] const Grasses& GetGrasses() const { return grasses;}
    void activateTextures(Shader& shader);

    const float GetHeightOfTerrain(float x, float z) const;
};



#endif //TERRAIN_H
