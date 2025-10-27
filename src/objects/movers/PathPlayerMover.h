

#ifndef PATHPLAYERMOVER_H
#define PATHPLAYERMOVER_H
#include <filesystem>
#include "../Player.h"
#include "../../pathing/Path.h"
#include <data_dir.h>

class PathPlayerMover {
private:
    Player& player;
    const Terrain& terrain;
    Path path;
    glm::vec3 movingTowards;
    int pointer = 1;
    void setToStart();
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
