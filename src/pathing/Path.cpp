

#include "Path.h"

#include "../images/Image.h"
#include "../terrain/Terrain.h"

// TODO: Is this the good place for config?
float distance = 2.5;
int raysPerIteration = 180;
int seeks = 300;

void Path::generatePath(glm::vec3 startPosition, const std::filesystem::path& blendMap, int terrainSize) {
    path = {
        startPosition
    };

    // we need to "shoot" X rays in each direction
    auto blendMapImage = Image(blendMap);
    int imageRatio = blendMapImage.getWidth() / terrainSize;

    float startingAngle = -90.0f;
    glm::vec3 latestPoint = startPosition;
    for (int j = 0; j < seeks; j++) {
        for (int i = 0; i < raysPerIteration; i++) {
            float angle = startingAngle + i;
            double radians = angle * (M_PI / 180.0f);
            float xPositionOnTheTerrain = sin(radians) * distance + latestPoint.x;
            float zPositionOnTheTerrain = -(cos(radians) * distance) + latestPoint.z;

            int blendMapImageXpos = xPositionOnTheTerrain * imageRatio;
            int blendMapImageZpos = -zPositionOnTheTerrain * imageRatio;

            // if any point has red color, we take that point as next point and we move on
            if (blendMapImage.isRedColor(blendMapImageXpos, blendMapImageZpos)) {
                latestPoint = glm::vec3(xPositionOnTheTerrain, 0.0f, zPositionOnTheTerrain);
                path.push_back(latestPoint);
                break;
            }
        }
    }
}
