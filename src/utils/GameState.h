#ifndef MYGAME_GAMESTATE_H
#define MYGAME_GAMESTATE_H

class GameState {
public:
    explicit GameState() : state(IN_MENU) {}
    void changeToInMenu();
    void changeToInGame();
    bool isInMenu() const {return state == IN_MENU;}
    bool isInGame() const {return state == IN_GAME;}
private:
    enum State {
        IN_MENU,
        IN_GAME,
    };
    State state;
};


#endif //MYGAME_GAMESTATE_H