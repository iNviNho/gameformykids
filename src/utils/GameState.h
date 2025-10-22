#ifndef MYGAME_GAMESTATE_H
#define MYGAME_GAMESTATE_H
#include "../audio/SoundManager.h"

class GameState {
public:
    explicit GameState(SoundManager& soundManager)
    : state(IN_MENU_GAME_NOT_STARTED), soundManager(soundManager) {
        changeToInMenuGameNotStarted();
    }
    void changeToInMenuGameNotStarted() {
        state = IN_MENU_GAME_NOT_STARTED;
        soundManager.playSound(SoundManager::MENU_INTRO);
    }
    void changeToInMenuGameAlreadyStarted() {
        state = IN_MENU_GAME_ALREADY_STARTED;
        soundManager.playSound(SoundManager::MENU_INTRO, false);
    }
    void changeToStartGame() {
        state = IN_GAME;
        soundManager.playSound(SoundManager::GAME_ELWYNN);
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
    SoundManager& soundManager;
};


#endif //MYGAME_GAMESTATE_H