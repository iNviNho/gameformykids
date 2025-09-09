

#ifndef PATHGENERATOR_H
#define PATHGENERATOR_H
#include <filesystem>
#include "../terrain/Terrain.h"
#include "glm/vec3.hpp"


class Path {
private:
    std::vector<glm::vec3> path;
    void generatePath(
        const std::string& pathFile,
        const float& pathFoundationSize,
        const float& offsetX,
        const float& offsetZ,
        const int& terrainSize
    );
public:
    explicit Path(
        const std::string& pathFile,
        const float& pathFoundationSize,
        const float& pathFoundationOffsetX,
        const float& pathFoundationOffsetZ,
        const int& terrainSize
    ) {
        generatePath(pathFile, pathFoundationSize, pathFoundationOffsetX, pathFoundationOffsetZ, terrainSize);
    }
    ~Path() = default;
    const std::vector<glm::vec3>& getPath() const { return path; }
};



#endif //PATHGENERATOR_H
