

#ifndef PATHPLAYERMOVER_H
#define PATHPLAYERMOVER_H
#include <filesystem>
#include <optional>
#include <array>
#include "../Player.h"
#include "../../pathing/Path.h"
#include <data_dir.h>
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
    glm::vec3 movingTowards;
    std::vector<glm::vec3>::const_iterator nextWaypoint;
    std::vector<Intersection> inters;
    std::vector<Intersection>::const_iterator nextInter;
    MeshIntersecter intersecter;
    void setToStart();

    glm::vec3 addHeight(const glm::vec3& point) const {
        return glm::vec3{ point.x, terrain.GetHeightOfTerrain(point.x, point.z), point.z };
    }

    void setMovingTowards(const glm::vec3& terrainSt, const glm::vec3& terrainEn);

    Player::State moveMovingPlayer(float& deltaTime, Player::Moving& moving);

    Player::State moveJumpingPlayer(float& deltaTime, Player::JumpingOnPath& jumping);

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
        nextWaypoint{ path.getPath().cbegin() },
        inters{},
        nextInter{ inters.cend() },
        intersecter{ terrain }
    {
        if (path.getPath().size() < 2) {
            throw std::runtime_error("Path does not have enough points. This happens if path couldn't be generated.");
        }
        // player starts at the first waypoint
        player.MoveTo(addHeight(*nextWaypoint));
        // he will move towards the next waypoint
        setMovingTowards(player.GetPosition(), addHeight(*(++nextWaypoint)));
        const glm::vec3 target = (nextInter == inters.cend()) ? movingTowards : nextInter->point;
		player.setState( Player::Moving{ DEFAULT_SPEED, target - player.GetPosition() });
        setToStart();
    }
    void move(float deltaTime);
    void Jump();
};



#endif //PATHPLAYERMOVER_H
