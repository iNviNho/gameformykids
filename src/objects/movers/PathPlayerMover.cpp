

#include "PathPlayerMover.h"

void PathPlayerMover::move(float deltaTime) {
    // calculate vec3 to move towards nextPoint
    glm::vec3 flatMovingTowards = movingTowards;
    flatMovingTowards.y = 1.0f;
    glm::vec3 flatPlayerPosition = player.GetPosition();
    flatPlayerPosition.y = 1.0f;
    glm::vec3 movePoint = flatMovingTowards - flatPlayerPosition;

    player.Move(movePoint, 4.0f * deltaTime);

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
