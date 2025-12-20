#include "SceneModifier.h"

#include <random>

#include "data_dir.h"
#include "../menu/Menu.h"
#include "../models/ModelGenerator.h"
#include "../utils/Log.h"

void SceneModifier::placeObject() {
    // Get the camera position and direction
    glm::vec3 camPos = camera.Position;
    glm::vec3 camDir = camera.GetFrontVector();

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
            Entity entity{
                modelsHolder.GetModel(selectedEntityName),
                glm::vec3(newPosition.x, terrainHeight, newPosition.z)
            };

            persist(entity, "grass");
            Log::logInfo("Saved to file");

            entitiesHolder.AddEntity(entity);

            break;
        }
    }
}

void SceneModifier::ChangeSelectedEntityName() {
    // get available names from models holder
    auto modelNames = modelsHolder.GetModelNames();
    // get index of current
    auto it = std::find(modelNames.begin(), modelNames.end(), selectedEntityName);
    // if we found it
    if (it != modelNames.end()) {
        // increase
        ++it;
        // doesn't exist?
        if (it == modelNames.end()) {
            // we point to first element
            it = modelNames.begin();
        }
        selectedEntityName = *it;
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
