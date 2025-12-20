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
    selectedEntityName("grass"){}

    void placeObject();
    void ChangeSelectedEntityName();
    std::string GetSelectedEntityName() const {
        return selectedEntityName;
    }
private:
    Camera& camera;
    Terrain& terrain;
    EntitiesHolder& entitiesHolder;
    LocalStorage& localStorage;
    ModelsHolder& modelsHolder;
    std::string selectedEntityName;

    void persist(Entity& entity, const std::string& name);
};


#endif //MYGAME_SCENEMODIFIER_H