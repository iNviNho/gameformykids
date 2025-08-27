

#ifndef PATHGENERATOR_H
#define PATHGENERATOR_H
#include <filesystem>
#include "../terrain/Terrain.h"
#include "glm/vec3.hpp"


class Path {
private:
    std::vector<glm::vec3> path;
    void generatePath(glm::vec3 startPosition, const std::filesystem::path& blendMap, int terrainSize);
public:
    explicit Path(
        const glm::vec3 startPosition,
        const std::filesystem::path& blendMap,
        const int terrainSize
    ) {
        generatePath(startPosition, blendMap, terrainSize);
    }
    ~Path() = default;
    const std::vector<glm::vec3>& getPath() const { return path; }
};



#endif //PATHGENERATOR_H
