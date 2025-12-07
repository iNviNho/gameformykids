#include "SceneModifier.h"

#include <random>

#include "data_dir.h"
#include "../menu/Menu.h"
#include "../models/ModelGenerator.h"
#include "../utils/Log.h"

void SceneModifier::placeObject() {
    Log::logInfo("Placing an object");

    // Get the camera position and direction
    glm::vec3 camPos = camera.Position;
    glm::vec3 camDir = camera.GetFrontVector();

    Log::logInfo("Camera position: (" + std::to_string(camPos.x) + ", " + std::to_string(camPos.y) + ", " + std::to_string(camPos.z) + ")");
    Log::logInfo("Camera direction: (" + std::to_string(camDir.x) + ", " + std::to_string(camDir.y) + ", " + std::to_string(camDir.z) + ")");

    // now we need to find the point on the terrain where the camera is looking at
    // we can do this by casting a ray from the camera position in the direction of the camera direction
    // and finding the intersection with the terrain

    float stepSize = 0.05f; // step size for ray marching
    int maxIterations = 10000;
    // we will march along the ray until we find we are below terrain
    for (int i = 1; i < maxIterations; ++i) {
        glm::vec3 newPosition = camPos + camDir * (i * stepSize);
        float terrainHeight = terrain.GetHeight(newPosition.x, newPosition.z);
        if (newPosition.y <= terrainHeight) {
            // we found the intersection point
            Log::logInfo("Placing object at: (" + std::to_string(newPosition.x) + ", " + std::to_string(terrainHeight) + ", " + std::to_string(newPosition.z) + ")");
            // Here you would create and place your object at (newPosition.x, terrainHeight, newPosition.z)

            std::shared_ptr<Model> grass = std::make_shared<Model>(
                data_dir() /= path("resources/objects/grass6/grass.obj")
            );

            Entity entity{grass, glm::vec3(newPosition.x, terrainHeight, newPosition.z)};

            Log::log("Saving to file first");
            persist(entity, "grass");
            Log::log("Saved to file");

            entitiesHolder.AddEntity(entity);

            break;
        }
    }
}

void SceneModifier::persist(Entity& entity, const std::string& name) {
    // name + unique identifier + entity
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned long long> dist;

    unsigned long long id = dist(gen);
    std::string data = name + "," + std::to_string(id) + "," + entity.toString();

    localStorage.persist(data);
}
