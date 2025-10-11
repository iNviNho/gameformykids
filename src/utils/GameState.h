#ifndef MYGAME_GAMESTATE_H
#define MYGAME_GAMESTATE_H

class GameState {
public:
    explicit GameState() : state(IN_MENU_GAME_NOT_STARTED) {}
    void changeToInMenuGameAlreadyStarted() {
        state = IN_MENU_GAME_ALREADY_STARTED;
    }
    void changeToStartGame() {
        state = IN_GAME;
    }
    bool isInMenuAndGameDidNotStart() const {return state == IN_MENU_GAME_NOT_STARTED;}
    bool isInMenuAndGameAlreadyStarted() const {return state == IN_MENU_GAME_ALREADY_STARTED;}
private:
    enum State {
        IN_MENU_GAME_NOT_STARTED,
        IN_MENU_GAME_ALREADY_STARTED,
        IN_GAME,
    };
    State state;
};


#endif //MYGAME_GAMESTATE_H