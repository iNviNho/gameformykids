

#ifndef PATHPLAYERMOVER_H
#define PATHPLAYERMOVER_H
#include <filesystem>
#include "../Player.h"
#include "../../pathing/Path.h"
#include <data_dir.h>

class PathPlayerMover {
private:
    Player& player;
    Path path;
    glm::vec3 movingTowards{};
    int pointer = 1;
    void setToStart();
public:
    explicit PathPlayerMover(Player& player, int terrainSize):
        player(player),
        path(Path{
            player.GetPosition(),
            data_dir() /= std::filesystem::path("resources/images/route.png"),
            terrainSize}
        ) {
        if (path.getPath().size() < pointer) {
            throw std::runtime_error("Pointer bigger than path size. This happens if path couldn't be generated.");
        }
        // player starts at pointer 0
        player.SetPosition(path.getPath().at(0));
        // he will move towards pointer 1
        movingTowards = path.getPath().at(1);
        setToStart();
    }
    void move(float deltaTime);
};



#endif //PATHPLAYERMOVER_H
