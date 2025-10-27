

#ifndef PATHPLAYERMOVER_H
#define PATHPLAYERMOVER_H
#include <filesystem>
#include <stdexcept>
#include <glm/vec3.hpp>
#include "../Player.h"
#include "../../pathing/Path.h"
#include <data_dir.h>
#include "../../terrain/Terrain.h"
#include "Intersection.h"
#include "MeshIntersecter.h"

class PathPlayerMover {
private:
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
        setToStart();
    }
    void move(float distance);
};



#endif //PATHPLAYERMOVER_H
