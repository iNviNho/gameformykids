#ifndef TERRAIN_H
#define TERRAIN_H
#include "Grasses.h"
#include "../images/Image.h"
#include "../models/Model.h"
#include "../shaders/shader.h"


class Terrain
{
private:
    static constexpr int SIZE = 256;
    static constexpr float MAX_HEIGHT = 30.0f;

    float* dataPoints;
    unsigned int VAO, VBO;
    unsigned int grassTexture;
    unsigned int pathTexture;
    unsigned int mudTexture;
    unsigned int flowersTexture;
    unsigned int blendMapTexture;
    Image* heightMap;
    Image* blendMap;

    void parseHeightMap(char const *heightMap);
    void parseBlendMap(char const* blendMap);
    void generateTextures();
    void generateVaoVbo();

    void generateTerrain();
    const float getHeight(float x, float z) const;
    glm::vec3 calculateNormal(float x, float z);

    Grasses grasses;
    void generateGrasses();

public:
    Terrain(char const* heightMap, char const* blendMap);
    ~Terrain() = default;
    [[nodiscard]] const float* GetDataPoints() const;
    [[nodiscard]] const float GetCountOfVertices() const;
    [[nodiscard]] long GetDataPointsSize() const;
    [[nodiscard]] unsigned int GetVAO() const { return VAO; }
    [[nodiscard]] unsigned int GetGrassTexture() const { return grassTexture; }
    [[nodiscard]] unsigned int GetPathTexturre() const { return pathTexture; }
    [[nodiscard]] unsigned int GetMudTexture() const { return mudTexture; }
    [[nodiscard]] unsigned int GetFlowersTexture() const { return flowersTexture; }
    [[nodiscard]] unsigned int GetBlendMapTexture() const { return blendMapTexture; }
    [[nodiscard]] float GetSize() const { return SIZE; }
    [[nodiscard]] const Grasses& GetGrasses() const { return grasses;}
    void activateTextures(Shader* shader);

    const float GetHeightOfTerrain(float x, float z) const;
};



#endif //TERRAIN_H
