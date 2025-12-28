#ifndef MYGAME_SCENEMODIFIER_H
#define MYGAME_SCENEMODIFIER_H
#include "../camera/camera.h"
#include "../models/ModelsHolder.h"
#include "../storage/LocalStorage.h"
#include "../terrain/Terrain.h"


class SceneModifier {
public:
    SceneModifier(
        Camera& camera,
        Terrain& terrain,
        EntitiesHolder& entitiesHolder,
        LocalStorage& localStorage,
        ModelsHolder& modelsHolder
    )
    : camera(camera),
    terrain(terrain),
    entitiesHolder(entitiesHolder),
    localStorage(localStorage),
    modelsHolder(modelsHolder),
    selectedEntityName("grass"),
    selectedScale(1.0f),
    selectedRotation(glm::vec3(1.0f)),
    previewEntity(Entity{
        modelsHolder.GetModel("grass"),
        glm::vec3(1.0f, 1.0f, 1.0f)
    }){}

    void placeObject();
    void ChangeSelectedEntityName();
    std::string GetSelectedEntityName() const {
        return selectedEntityName;
    }
    float GetScale() const {
        return selectedScale;
    }
    glm::vec3 GetRotation() const {
        return selectedRotation;
    }
    void increaseScale() {
        selectedScale = selectedScale + 0.1f;
    }
    void decreaseScale() {
        selectedScale = selectedScale - 0.1f;
        if (selectedScale < 0.1f) {
            selectedScale = 0.1f;
        }
    }
    void increaseRotationX() {
        selectedRotation.x += 2.0f;
    }
    void decreaseRotationX() {
        selectedRotation.x -= 2.0f;
    }
    void increaseRotationY() {
        selectedRotation.y += 2.0f;
    }
    void decreaseRotationY() {
        selectedRotation.y -= 2.0f;
    }
    void increaseRotationZ() {
        selectedRotation.z += 2.0f;
    }
    void decreaseRotationZ() {
        selectedRotation.z -= 2.0f;
    }
    Entity& GetSelectedEntityPreviewEntity();

private:
    Camera& camera;
    Terrain& terrain;
    EntitiesHolder& entitiesHolder;
    LocalStorage& localStorage;
    ModelsHolder& modelsHolder;
    std::string selectedEntityName;
    float selectedScale;
    glm::vec3 selectedRotation;
    Entity previewEntity;

    void persist(Entity& entity, const std::string& name);
    void applySelectedTransform(Entity& entity);
    std::optional<glm::vec3> raycastToTerrain(float stepSize = 0.05f, int maxIterations = 10000) const;
};


#endif //MYGAME_SCENEMODIFIER_H