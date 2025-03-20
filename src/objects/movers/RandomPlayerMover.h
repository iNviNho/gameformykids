

#ifndef PLAYERMOVER_H
#define PLAYERMOVER_H
#include "../Player.h"


class RandomPlayerMover {
private:
    Player& player;
public:
    explicit RandomPlayerMover(Player& player): player(player) {}
    void move(float deltaTime);
};



#endif //PLAYERMOVER_H
