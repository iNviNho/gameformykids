#ifndef TERRAIN_H
#define TERRAIN_H
#include "../images/Image.h"


class Terrain
{
private:
    static constexpr int SIZE = 256;
    static constexpr float MAX_HEIGHT = 20.0f;
    unsigned int xPos;
    unsigned int zPos;

    float* dataPoints;
    unsigned int VAO, VBO;
    unsigned int texture;
    Image* heightMap;

    void parseHeightMap(char const* heightMap);
    void generateTexture();
    void generateVaoVbo();
    void generateTerrain();
    float getHeight(float x, float z);
public:
    Terrain(int xPos, int yPos, char const* heightMap);
    ~Terrain() = default;
    [[nodiscard]] const float* GetDataPoints() const;
    [[nodiscard]] const float GetCountOfVertices() const;
    [[nodiscard]] long GetDataPointsSize() const;
    [[nodiscard]] unsigned int GetVAO() const { return VAO; }
    [[nodiscard]] unsigned int GetTexture() const { return texture; }
};



#endif //TERRAIN_H
