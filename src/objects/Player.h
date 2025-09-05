

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
    bool jumped = false;
    double jumpedAt = 0.0f;
    float speed = 4.0f;
    void updateCameraPitch();
public:
    Player(Camera &camera, Terrain &terrain, const std::shared_ptr<Model> &model, glm::vec3 position):
        Entity(model, position), terrain(terrain), camera(camera) {
    }
    void MoveBy(glm::vec3& moveVector);
    void MoveIn(const glm::vec3& dir, float& distance);

    void UpdateCameraPosition(bool animated = true);
    void UpdateCameraYaw(bool animated = true);


    void Jump();
    void handleJump(float deltaTime);

    float GetSpeed() const { return speed;}
    Camera& GetCamera() { return camera;}
};

#endif //PLAYER_H
