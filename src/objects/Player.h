

#ifndef PLAYER_H
#define PLAYER_H
#include "Entity.h"
#include "../camera/camera.h"
#include "../terrain/Terrain.h"


class Player: public Entity {
    using Entity::Entity;
private:
    Camera &camera;
    Terrain &terrain;
    void updateCameraPosition();
    void updateCameraPitch();
    void updateCameraYaw();
public:
    Player(Camera &camera, Terrain &terrain, Model &model, glm::vec3 position):
        Entity(model, position), terrain(terrain), camera(camera) {
        updateCameraPitch();
        updateCameraYaw();
    }
    void Move(glm::vec3 pos);
    void Move(glm::vec3 dir, float distance);
    void SetRotateY(float angle);
    void SetRotateX(float angle);
};

#endif //PLAYER_H
