

#ifndef PATHPLAYERMOVER_H
#define PATHPLAYERMOVER_H
#include <filesystem>
#include <stdexcept>
#include <glm/vec3.hpp>
#include <optional>
#include <array>
#include <glm/vec4.hpp>
#include "../Player.h"
#include "../../pathing/Path.h"
#include <data_dir.h>
#include "../../terrain/Terrain.h"
#include "Intersection.h"
#include "MeshIntersecter.h"

class PathPlayerMover {
private:

    static constexpr float DEFAULT_SPEED = 4.0f;

    // for higher jumps, increase
    static constexpr glm::vec3 INITIAL_JUMP_VELOCITY{ 0, 5.0f, 0 };

    Player& player;
    const Terrain& terrain;
    Path path;
    // the point we are currently moving towards (no matter the amount of intersections)
    glm::vec3 movingTowards;
    std::vector<glm::vec3>::const_iterator nextWaypoint;
    std::vector<Intersection> intersections;
    std::vector<Intersection>::const_iterator nextIntersection;
    MeshIntersecter intersecter;

    glm::vec3 addHeight(const glm::vec3& point) const {
        return glm::vec3{ point.x, terrain.GetHeightOfTerrain(point.x, point.z), point.z };
    }

    void setMovingTowards(const glm::vec3& terrainSt, const glm::vec3& terrainEn);

    constexpr Player::State move(Player::Stationary& stationary, float& deltaTime) noexcept {
        deltaTime = 0.0f;
        return player.getState();
    }

    Player::State move(Player::Moving& moving, float& deltaTime);

    Player::State move(Player::JumpingOnPath& jumping, float& deltaTime);

    static Intersection planeTrajIntersection(const glm::vec4& plane, const glm::vec3& pos, const glm::vec3& v);

    static std::array<float, 2> solveQuadratic(const float a, const float b, const float c);

    static float getLandingTime(const std::array<float, 2>& solutions);

public:
    explicit PathPlayerMover(Player& player, const Terrain& terrain):
        player(player),
        terrain(terrain),
        path(Path{
            data_dir() /= std::filesystem::path("resources/paths/path.svg"),
            1024.0f,
            90.0f,
            110.0f,
            terrain.GetSize()
        }),
        // we are pointing to the beginning of the path
        nextWaypoint{ path.getPath().cbegin() },
        // let's initialize intersections
        intersections{},
        // we are pointing past the end of intersections
        // since intersections is empty, this defines empty state
        nextIntersection{ intersections.cend() },
        // initialize intersecter
        intersecter{ terrain }
    {
        if (path.getPath().size() < 2) {
            throw std::runtime_error("Path does not have enough points. This happens if path couldn't be generated.");
        }
        // player starts at the first waypoint
        player.MoveTo(addHeight(*nextWaypoint));
        // he will move towards the next waypoint
        setMovingTowards(player.GetPosition(), addHeight(*(++nextWaypoint)));
        // we check if we are moving towards intersection or to the waypoint
        const glm::vec3 target = (nextIntersection == intersections.cend()) ? movingTowards : nextIntersection->point;
        const glm::vec3 dir = target - player.GetPosition();
		player.setState( Player::Moving{ DEFAULT_SPEED, dir });
		player.updateRotation(dir);
        player.UpdateCameraPose(false);
    }
    void move(float deltaTime);
    void Jump();
};



#endif //PATHPLAYERMOVER_H
