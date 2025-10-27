

#include "PathPlayerMover.h"
#include <algorithm>
#include <variant>
#include <cmath>
#include <glm/geometric.hpp>
#include "../../utils/Log.h"

static constexpr float AT_TARGET = 1e-5f;

static constexpr float GRAVITY = -9.8f;

void PathPlayerMover::move(float deltaTime) {
    while (deltaTime > 0.0f) {
        Player::State& state = player.getState();
        if (std::holds_alternative<Player::Moving>(state))
            player.setState(move(std::get<Player::Moving>(state), deltaTime));
        else if (std::holds_alternative<Player::JumpingOnPath>(state))
            player.setState(move(std::get<Player::JumpingOnPath>(state), deltaTime));
        else if (std::holds_alternative<Player::Stationary>(state))
            player.setState(move(std::get<Player::Stationary>(state), deltaTime));
        else
            throw std::logic_error("Invalid player state");
    }
}

void PathPlayerMover::Jump() {
    Player::State& state = player.getState();
    if (std::holds_alternative<Player::Moving>(state)) {
        Log::logInfo("Player jumped from moving");
        const Player::Moving& movingState = std::get<Player::Moving>(state);
        player.setState(Player::State{ Player::JumpingOnPath{ player.GetPosition(), movingState.speed, (movingState.dir * movingState.speed).y + INITIAL_JUMP_VELOCITY.y } });
    }
    else if (std::holds_alternative<Player::Stationary>(state)) {
        Log::logInfo("Player jumped from stationary");
        player.setState(Player::State{ Player::JumpingOnPath{ player.GetPosition(), 0,  INITIAL_JUMP_VELOCITY.y } });
    }
    else if (std::holds_alternative<Player::JumpingOnPath>(state)) {
        return;
    }
    else
        throw std::logic_error("Invalid player state");
}

Intersection PathPlayerMover::planeTrajIntersection(const glm::vec4& plane, const glm::vec3& pos, const glm::vec3& v)
{
    // x(t) = at + b
    const float a = v.x;
    const float b = pos.x;

    // y(t) = ct^2 + dt + e
    const float c = GRAVITY;
    const float d = v.y;
    const float e = pos.y;

    // z(t) = ft + g
    const float f = v.z;
    const float g = pos.z;

    // quadratic equation
    // lt^2 + mt + n = 0
    const float l = plane.y * GRAVITY;
    const float m = plane.x * a + plane.y * d + plane.z * f;
    const float n = plane.x * b + plane.y * e + plane.z * g + plane.w;

    const std::array<float, 2> sols = solveQuadratic(l, m, n);

    const float t = getLandingTime(sols);
    return Intersection{ glm::vec3{ a * t + b, c * t * t + d * t + e, f * t + g }, t };
}

std::array<float, 2> PathPlayerMover::solveQuadratic(const float a, const float b, const float c)
{
    const float s_arg = b * b - 4 * a * c;
    if (s_arg < 0.0)
        throw std::runtime_error("No real solutions to the quadratic equation");
    const float s = std::sqrt(s_arg);
    const float denom = 2 * a;

    std::array<float, 2> sols{ (-b + s) / denom, (-b - s) / denom };
    std::sort(sols.begin(), sols.end());
    return sols;
}

float PathPlayerMover::getLandingTime(const std::array<float, 2>& solutions)
{
    if (solutions[1] < 0)
        throw std::logic_error("Landed in the past");
    return solutions[1];
}

Player::State PathPlayerMover::move(Player::Moving& moving, float& deltaTime) {

    // check if we are at the end of the path
    if (nextWaypoint == path.getPath().cend())
        return Player::State{ Player::Stationary{} };

    glm::vec3 current = player.GetPosition();
    std::optional<glm::vec3> updatedDir{};

    float distance = deltaTime * moving.speed;
    while (distance > 0) {

        glm::vec3 target;
        float distToTarget;
        if (nextInter == inters.cend()) {
            target = movingTowards;
            distToTarget = glm::distance(current, target);

            if (distToTarget < AT_TARGET) {
                // check if we are at the end of the path
                if (++nextWaypoint == path.getPath().cend()) {
                    deltaTime = distance / moving.speed;
                    if (updatedDir)
                        player.MoveTo(current, *updatedDir);
                    else
                        player.MoveTo(current);
                    return Player::State{ Player::Stationary{} };
                }
                setMovingTowards(current, addHeight(*nextWaypoint));

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
        updatedDir = dir;

        const float toTarget = std::min(distToTarget, distance);

        current += dir * toTarget;

        distance -= toTarget;
    }
    deltaTime = 0.0f;
    if (updatedDir) {
        moving.dir = *updatedDir;
        player.MoveTo(current, moving.dir);
    }
    else
        player.MoveTo(current);

    return player.getState();
}

Player::State PathPlayerMover::move(Player::JumpingOnPath& jumping, float& deltaTime) {

    glm::vec3 pos = player.GetPosition();

    if (pos.y < jumping.terrainPos.y)
        throw std::logic_error("Player below terrain at start");

    std::optional<glm::vec3> updatedDir{};

    std::optional<Intersection> landing{};

    // check if we are at the end of the path
    if (nextWaypoint == path.getPath().cend()) {
        const float terrain_h = jumping.terrainPos.y;
        const float player_h = pos.y + jumping.velocity * deltaTime + GRAVITY * deltaTime * deltaTime;
        if (player_h <= terrain_h) {
            // hit the terrain in this time interval, solve for t in y(t) = terrain_h
            const float t = getLandingTime(solveQuadratic(GRAVITY, jumping.velocity, pos.y - terrain_h));
            if (t > deltaTime)
                throw std::logic_error("Expected t <= deltaTime");
            landing = Intersection{ jumping.terrainPos, t };
        }
        else {
            pos.y += deltaTime * jumping.velocity + GRAVITY * deltaTime * deltaTime;
            jumping.velocity += GRAVITY * deltaTime;
            deltaTime = 0.0f;
        }
    }
    else {
        while (deltaTime > 0) {

            glm::vec3 target;
            float distToTarget;
            if (nextInter == inters.cend()) {
                target = movingTowards;
                distToTarget = glm::distance(jumping.terrainPos, target);

                if (distToTarget < AT_TARGET) {
                    // check if we are at the end of the path
                    if (++nextWaypoint == path.getPath().cend()) {
                        const float terrain_h = target.y;
                        const float player_h = pos.y + jumping.velocity * deltaTime + GRAVITY * deltaTime * deltaTime;
                        if (player_h <= terrain_h) {
                            // hit the terrain in this time interval, solve for t in y(t) = terrain_h
                            const float t = getLandingTime(solveQuadratic(GRAVITY, jumping.velocity, pos.y - terrain_h));
                            if(t > deltaTime)
                                throw std::logic_error("Expected t <= deltaTime");
                            landing = Intersection{ target, t };
                            break;
                        }
                        pos.y += deltaTime * jumping.velocity + GRAVITY * deltaTime * deltaTime;
                        jumping.velocity += GRAVITY * deltaTime;
                        deltaTime = 0.0f;
                        break;
                    }

                    setMovingTowards(jumping.terrainPos, addHeight(*nextWaypoint));

                    if (nextInter == inters.cend())
                        target = movingTowards;
                    else
                        target = nextInter->point;
                    distToTarget = glm::distance(jumping.terrainPos, target);
                }
            }
            else {
                target = nextInter->point;
                distToTarget = glm::distance(jumping.terrainPos, target);

                if (distToTarget < AT_TARGET) {
                    if (++nextInter == inters.cend())
                        target = movingTowards;
                    else
                        target = nextInter->point;
                    distToTarget = glm::distance(jumping.terrainPos, target);
                }
            }

            // normalized direction to target
            const glm::vec3 dir = (target - jumping.terrainPos) / distToTarget;
            updatedDir = dir;

            // determine the triangle where the player is from the midpoint between
            // the current terrain position and the next terrain target
            const float cx = (jumping.terrainPos.x + target.x) / 2.0f;
            const float cz = (jumping.terrainPos.z + target.z) / 2.0f;
            const std::array<glm::vec3, 3> triangle = terrain.GetTriangle(cx, cz);
            const glm::vec4 plane = Terrain::GetTrianglePlane(triangle);

            const Intersection inter = planeTrajIntersection(plane, pos, glm::vec3{ dir.x * jumping.terrainSpeed,
                                                                                    jumping.velocity,
                                                                                    dir.z * jumping.terrainSpeed });
            if (Terrain::IsInsideTriangle(triangle, glm::vec3{ plane.x, plane.y, plane.z }, inter.point)) {
                if (inter.t <= deltaTime) {
                    landing = inter;
                    break;
                }
                else {
                    // landed outside the current time interval
                    const float maxT = std::min(deltaTime, distToTarget / jumping.terrainSpeed);

                    jumping.terrainPos.x += dir.x * maxT * jumping.terrainSpeed;
                    jumping.terrainPos.z += dir.z * maxT * jumping.terrainSpeed;
                    jumping.terrainPos.y = Terrain::GetHeightOfTerrain(plane, jumping.terrainPos.x, jumping.terrainPos.z);

                    pos.x = jumping.terrainPos.x;
                    pos.z = jumping.terrainPos.z;
                    pos.y += maxT * jumping.velocity + GRAVITY * maxT * maxT;

                    if (pos.y < jumping.terrainPos.y)
                        throw std::logic_error("Player below terrain");

                    jumping.velocity += GRAVITY * maxT;

                    deltaTime -= maxT;
                }
            }
            else {
                const float maxT = std::min(deltaTime, distToTarget / jumping.terrainSpeed);

                jumping.terrainPos += dir * maxT * jumping.terrainSpeed;

                pos.x = jumping.terrainPos.x;
                pos.z = jumping.terrainPos.z;
                pos.y += maxT * jumping.velocity + GRAVITY * maxT * maxT;

                if (pos.y < jumping.terrainPos.y)
                    throw std::logic_error("Player below terrain");

                jumping.velocity += GRAVITY * maxT;

                deltaTime -= maxT;
            }
        }
    }

    if (!landing) {
        const glm::vec3 endPos{ jumping.terrainPos.x, pos.y, jumping.terrainPos.z };
        if (updatedDir)
            player.MoveTo(endPos, *updatedDir);
        else
            player.MoveTo(endPos);
    }
    else {
        const glm::vec3& newPos = landing->point;
        deltaTime -= landing->t;

        if (updatedDir)
            player.MoveTo(newPos, *updatedDir);
        else
            player.MoveTo(newPos);

        const Player::State& prevState = player.getPrevState();
        if (std::holds_alternative<Player::Stationary>(prevState)) {
            Log::logInfo("Player stationary from jumping");
            return prevState;
        }
        else if (std::holds_alternative<Player::Moving>(prevState)) {
            Log::logInfo("Player moving from jumping");
            const Player::Moving& lastMoving = std::get<Player::Moving>(prevState);

            glm::vec3 movingDir;
            if (updatedDir)
                movingDir = *updatedDir;
            else {
                const float angle = player.GetRotationYAngle() * (M_PI / 180.0f);
                movingDir.x = std::cos(angle);
                movingDir.z = std::sin(angle);
                movingDir.y = 0.0f;
            }

            return Player::State{ Player::Moving{ lastMoving.speed, movingDir} };
        }
        else
            throw std::logic_error("Invalid previous player state");
    }

    return player.getState();
}

void PathPlayerMover::setMovingTowards(const glm::vec3& terrainSt, const glm::vec3& terrainEn)
{
    movingTowards = terrainEn;
    inters.clear();
    intersecter.computeIntersections(glm::vec2{ terrainSt.x, terrainSt.z }, glm::vec2{ terrainEn.x, terrainEn.z }, inters);
    nextInter = inters.cbegin();
}
