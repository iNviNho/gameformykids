

#ifndef PATHPLAYERMOVER_H
#define PATHPLAYERMOVER_H
#include <filesystem>
#include <stdexcept>
#include <glm/vec3.hpp>
#include "../Player.h"
#include "../../pathing/Path.h"
#include <data_dir.h>

class PathPlayerMover {
private:
    Player& player;
    Path path;
    glm::vec3 movingTowards;
    int pointer = 1;
    void setToStart();
public:
    explicit PathPlayerMover(Player& player, int terrainSize):
        player(player),
        path(Path{
            data_dir() /= std::filesystem::path("resources/paths/path.svg"),
            1024.0f,
            90.0f,
            110.0f,
            terrainSize
        })
    {
        if (path.getPath().size() < pointer) {
            throw std::runtime_error("Pointer bigger than path size. This happens if path couldn't be generated.");
        }
        // player starts at pointer 0
        player.MoveTo(path.getPath().at(0));
        // he will move towards vector at position 1
        movingTowards = path.getPath().at(1);
        setToStart();
    }
    void move(float distance);
};



#endif //PATHPLAYERMOVER_H
