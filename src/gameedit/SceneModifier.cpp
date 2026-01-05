#include "SceneModifier.h"

#include <random>

#include "../utils/Log.h"

constexpr float removalRadius = 0.5f;

std::optional<glm::vec3> SceneModifier::raycastToTerrain(
    float stepSize,
    int maxIterations
) const {
    // Get the camera position and direction
    const glm::vec3 camPos = camera.Position;
    const glm::vec3 camDir = camera.GetFrontVector();

    // now we need to find the point on the terrain where the camera is looking at
    // we can do this by casting a ray from the camera position in the direction of the camera direction
    // and finding the intersection with the terrain
    for (int i = 1; i < maxIterations; ++i) {
        const float distAlongRay = static_cast<float>(i) * stepSize;
        glm::vec3 pos = camPos + camDir * distAlongRay;
        float terrainHeight = terrain.GetHeight(pos.x, pos.z);

        if (pos.y <= terrainHeight) {
            // we found the intersection point
            return glm::vec3(pos.x, terrainHeight, pos.z);
        }
    }

    return std::nullopt;
}

void SceneModifier::applySelectedTransform(Entity& entity) const {
    entity.SetScale(GetScale());
    entity.SetRotateX(selectedRotation.x);
    entity.SetRotateY(selectedRotation.y);
    entity.SetRotateZ(selectedRotation.z);
}

void SceneModifier::placeObject() {
    auto hitPoint = raycastToTerrain();
    if (!hitPoint.has_value()) {
        Log::logWarning("Unable to find a hit point when placing object.");
        return;
    }

    const glm::vec3& position = hitPoint.value();

    Log::logInfo(
        "Placing object at: (" +
        std::to_string(position.x) + ", " +
        std::to_string(position.y) + ", " +
        std::to_string(position.z) + ")"
    );

    Entity entity{
        modelsHolder.GetModel(selectedEntityName),
        position
    };

    applySelectedTransform(entity);

    persist(entity, selectedEntityName);
    Log::logInfo("Object persisted to the storage.");

    entitiesHolder.AddEntity(entity);
}

void SceneModifier::removeObject() {
    // full deletion — in-memory + persisted storage
    auto hitPoint = raycastToTerrain();
    if (!hitPoint.has_value()) {
        Log::logWarning("Unable to find a hit point when removing object.");
        return;
    }

    const glm::vec3& position = hitPoint.value();
    Log::logInfo(
        "Removing object near: (" +
        std::to_string(position.x) + ", " +
        std::to_string(position.y) + ", " +
        std::to_string(position.z) + ")"
    );

    auto found = entitiesHolder.FindNearest(position, removalRadius);
    if (!found.has_value()) {
        Log::logWarning("No entity found within removal radius");
        return;
    }

    const glm::vec3 foundPosition = found.value();

    // remove from in-memory container
    const bool removedFromInMemory = entitiesHolder.RemoveByPosition(foundPosition);
    if (!removedFromInMemory) {
        Log::logWarning("Entity found but failed to remove from in-memory holder");
    } else {
        Log::logInfo("Entity removed from in-memory holder");
    }

    // remove from persistent storage
    try {
        const bool removedFromLocalStorage = localStorage.RemoveEntityByPosition(foundPosition);
        if (removedFromLocalStorage) {
            Log::logInfo("Entity removed from LocalStorage");
        } else {
            Log::logWarning("No matching persisted entity found to remove");
        }
    } catch (const std::exception& ex) {
        Log::logError(std::string("Failed to remove entity from LocalStorage: ") + ex.what());
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
    // we must recreate preview entity
    previewEntity = Entity{
        modelsHolder.GetModel(selectedEntityName),
        glm::vec3(1.0f, 1.0f, 1.0f)
    };
    selectedScale = 1.0f;
    selectedRotation = glm::vec3(1.0f);
}

Entity& SceneModifier::GetSelectedEntityPreviewEntity() {
    auto hitPoint = raycastToTerrain();
    if (hitPoint.has_value()) {
        previewEntity.MoveTo(hitPoint.value());
        applySelectedTransform(previewEntity);
    }

    return previewEntity;
}

void SceneModifier::persist(Entity& entity, const std::string& name) {
    // The idea is to store entity in the following format:
    // name,unique_identifier,entity.toString()
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned long long> dist;

    unsigned long long id = dist(gen);
    std::string data = name + "," + std::to_string(id) + "," + entity.toString();

    localStorage.persist(data);
}
