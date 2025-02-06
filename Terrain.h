//
// Created by Vladimír Vráb on 03.02.25.
//

#ifndef TERRAIN_H
#define TERRAIN_H



class Terrain
{
private:
    static constexpr int SIZE = 800;
    static constexpr int VERTEX_COUNT = 128;

    unsigned int xPos;
    unsigned int zPos;

    float* dataPoints;

    void generateTerrain();
public:
    Terrain(int xPos, int yPos);
    ~Terrain() = default;
    [[nodiscard]] const float* GetDataPoints() const;
    [[nodiscard]] const float GetCountOfVertices() const;
    [[nodiscard]] long GetDataPointsSize() const;
    [[nodiscard]] unsigned int XPos() const { return xPos; }
    [[nodiscard]] unsigned int ZPos() const { return zPos; }
};



#endif //TERRAIN_H
