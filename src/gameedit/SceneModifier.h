#ifndef MYGAME_SCENEMODIFIER_H
#define MYGAME_SCENEMODIFIER_H
#include "../camera/camera.h"
#include "../models/ModelsHolder.h"
#include "../storage/LocalStorage.h"
#include "../terrain/Terrain.h"
#include "../objects/Entity.h"
#include "../terrain/EntitiesHolder.h"

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
    selectedRadius(1),
    previewEntity(Entity{
        modelsHolder.GetModel("grass"),
        glm::vec3(1.0f, 1.0f, 1.0f)
    }){}

    void placeObject();
    void removeObject();
    void ChangeSelectedEntityName();
    void ModifyTerrainHeight(int direction);
    
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
        selectedScale = selectedScale + 0.05f;
    }
    void decreaseScale() {
        selectedScale = selectedScale - 0.05f;
        if (selectedScale < 0.05f) {
            selectedScale = 0.05f;
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
    void ModifySelectedRadius(int direction) {
        selectedRadius += 1 * direction;
        if (selectedRadius < 1) {
            selectedRadius = 1;
        }
    }
    int getSelectedRadius() {
        return selectedRadius;
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
    int selectedRadius;

    void persist(Entity& entity, const std::string& name);
    void applySelectedTransform(Entity& entity) const;
    std::optional<glm::vec3> raycastToTerrain(float stepSize = 0.05f, int maxIterations = 10000) const;
    bool removeEntityByPosition(glm::vec3 foundPosition, float epsilon = 1e-4f);
};


#endif //MYGAME_SCENEMODIFIER_H
