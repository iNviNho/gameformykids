

#include "PathPlayerMover.h"
#include <limits>
#include <algorithm>

static constexpr const float AT_TARGET = std::numeric_limits<float>::epsilon();

void PathPlayerMover::move(float distance) {
    while (distance > 0) {

        const glm::vec3& current = player.GetPosition();
        glm::vec3 target;
        float distToTarget;
        if (nextInter == inters.cend()) {
            target = movingTowards;
            distToTarget = glm::distance(current, target);

            if (distToTarget < AT_TARGET) {
                if (pointer >= path.getPath().size() - 1)
                    return; // we are at the end of the path
                setMovingTowards(addHeight(path.getPath().at(++pointer)));

                if (nextInter == inters.cend())
                    target = movingTowards;
                else
                    target = nextInter->point;
                distToTarget = glm::distance(current, target);
            }
        }
        else {
            target = nextInter->point;
            distToTarget = glm::distance(current, target);

            if (distToTarget < AT_TARGET) {
                if (++nextInter == inters.cend())
                    target = movingTowards;
                else
                    target = nextInter->point;
                distToTarget = glm::distance(current, target);
            }
        }

        // normalized direction to target
        const glm::vec3 dir = (target - current) / distToTarget;

        const float toTarget = std::min(distToTarget, distance);

        player.MoveIn(dir, toTarget);
 
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

void PathPlayerMover::setMovingTowards(const glm::vec3& point)
{
    movingTowards = point;
    const glm::vec3& pos = player.GetPosition();
    inters.clear();
    intersecter.computeIntersections(glm::vec2{ pos.x, pos.z }, glm::vec2{ movingTowards.x, movingTowards.z }, inters);
    nextInter = inters.cbegin();
}
