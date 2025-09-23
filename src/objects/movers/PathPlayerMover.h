

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
    int pointer = 1;
    std::vector<Intersection> inters;
    std::vector<Intersection>::const_iterator nextInter;
    MeshIntersecter intersecter;
    void setToStart();

    glm::vec3 addHeight(const glm::vec3& point) const {
        return glm::vec3{ point.x, terrain.GetHeightOfTerrain(point.x, point.z), point.z };
    }

    void setMovingTowards(const glm::vec3& point);

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
        inters{},
        nextInter{ inters.cend() },
        intersecter{ terrain }
    {
        if (path.getPath().size() < pointer) {
            throw std::runtime_error("Pointer bigger than path size. This happens if path couldn't be generated.");
        }
        // player starts at pointer 0
        player.MoveTo(addHeight(path.getPath().at(0)));
        // he will move towards vector at position 1
        setMovingTowards(addHeight(path.getPath().at(1)));
        setToStart();
    }
    void move(float distance);
};



#endif //PATHPLAYERMOVER_H
