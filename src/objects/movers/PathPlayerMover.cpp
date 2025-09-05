

#include "PathPlayerMover.h"

void PathPlayerMover::move(float deltaTime) {
    // calculate vec3 to move towards nextPoint
    glm::vec3 flatMovingTowards = movingTowards;
    flatMovingTowards.y = 1.0f;
    glm::vec3 flatPlayerPosition = player.GetPosition();
    flatPlayerPosition.y = 1.0f;
    glm::vec3 movePoint = flatMovingTowards - flatPlayerPosition;

    player.MoveIn(movePoint, deltaTime);

    if (abs(player.GetPosition().x - movingTowards.x ) < 0.5f &&
        abs(player.GetPosition().z - movingTowards.z) < 0.5f &&
        movingTowards.x == path.getPath().at(pointer).x
        ) {
        if (pointer + 1 < path.getPath().size()) {
            pointer++;
            movingTowards = path.getPath().at(pointer);
        }
    }
}

void PathPlayerMover::setToStart() {
    // we will move very little to trigger basic camera update
    move(0.01f);
    // now we move camera to where it should be without waiting for animation
    player.GetCamera().UpdatePosition(player.GetCamera().GetTargetPosition(), false);
    // and we update yaw to match the direction of the first path point
    player.GetCamera().UpdateYaw(player.GetCamera().GetTargetYaw(), false);
}
