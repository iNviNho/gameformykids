

#ifndef PLAYER_H
#define PLAYER_H
#include "Entity.h"
#include "../camera/camera.h"


class Player: public Entity {
    using Entity::Entity;
private:
    Camera &camera;
    void updateCameraPosition();
    void UpdateCameraPitch();
    void UpdateCameraYaw();
public:
    Player(Camera &camera, Model &model, glm::vec3 position):
        Entity(model, position), camera(camera) {}
    void Move(glm::vec3 pos);
    void SetRotateY(float angle);
    void SetRotateX(float angle);
};

#endif //PLAYER_H
