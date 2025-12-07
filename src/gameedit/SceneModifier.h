#ifndef MYGAME_SCENEMODIFIER_H
#define MYGAME_SCENEMODIFIER_H
#include "../camera/camera.h"
#include "../storage/LocalStorage.h"
#include "../terrain/Terrain.h"


class SceneModifier {
public:
    SceneModifier(Camera& camera, Terrain& terrain, EntitiesHolder& entitiesHolder, LocalStorage& localStorage)
    : camera(camera), terrain(terrain), entitiesHolder(entitiesHolder), localStorage(localStorage) {}

    void placeObject();
private:
    Camera& camera;
    Terrain& terrain;
    EntitiesHolder& entitiesHolder;
    LocalStorage& localStorage;

    void persist(Entity& entity, const std::string& name);
};


#endif //MYGAME_SCENEMODIFIER_H