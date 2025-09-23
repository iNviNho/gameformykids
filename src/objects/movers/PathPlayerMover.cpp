

#include "PathPlayerMover.h"
#include <limits>
#include <algorithm>

static constexpr const float AT_TARGET = std::numeric_limits<float>::epsilon();

void PathPlayerMover::move(float distance) {
    while (distance > 0) {

        const glm::vec2 flatCurrent{ player.GetPosition().x, player.GetPosition().z };

        glm::vec2 flatTarget{ movingTowards.x, movingTowards.z };
        float distToTarget = glm::distance(flatCurrent, flatTarget);
        if (distToTarget < AT_TARGET) {
            if(pointer >= path.getPath().size() - 1)
                return; // we are at the end of the path
            movingTowards = path.getPath().at(++pointer);
 
            flatTarget = glm::vec2{ movingTowards.x, movingTowards.z };
            distToTarget = glm::distance(flatCurrent, flatTarget);
        }

        // normalized direction to target
        const glm::vec2 dir = (flatTarget - flatCurrent) / distToTarget;

        const float toTarget = std::min(distToTarget, distance);

        player.MoveIn(glm::vec3{ dir.x, 0, dir.y }, toTarget);
 
        distance -= toTarget;
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
