

#ifndef PATHPLAYERMOVER_H
#define PATHPLAYERMOVER_H
#include <filesystem>
#include "../Player.h"
#include "../../pathing/Path.h"


class PathPlayerMover {
private:
    Player& player;
    Path path;
    glm::vec3 movingTowards{};
    int pointer = 1;
public:
    explicit PathPlayerMover(Player& player, int terrainSize):
        player(player), path(path) {
        path = Path(
            player.GetPosition(),
            std::filesystem::path("/Users/vladino/CLionProjects/mygame/resources/images/route.png"),
            terrainSize
        );
        // TODO: This may be NULL POINTER EXCEPTION
        movingTowards = path.getPath().at(pointer);
    }
    void move(float deltaTime);
};



#endif //PATHPLAYERMOVER_H
