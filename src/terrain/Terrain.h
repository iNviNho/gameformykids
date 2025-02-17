#ifndef TERRAIN_H
#define TERRAIN_H
#include "../images/Image.h"
#include "../shaders/shader.h"


class Terrain
{
private:
    static constexpr int SIZE = 128;
    static constexpr float MAX_HEIGHT = 20.0f;
    unsigned int xPos;
    unsigned int zPos;

    float* dataPoints;
    unsigned int VAO, VBO;
    unsigned int grassTexture;
    unsigned int pathTexture;
    unsigned int mudTexture;
    unsigned int flowersTexture;
    unsigned int blendMapTexture;
    Image* heightMap;

    void parseHeightMap(char const* heightMap);
    void generateTextures();
    void generateVaoVbo();

    void generateTerrain();
    float getHeight(float x, float z);
    glm::vec3 calculateNormal(float x, float z);
public:
    Terrain(int xPos, int yPos, char const* heightMap);
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
    void activateTextures(Shader* shader);
};



#endif //TERRAIN_H
