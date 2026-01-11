#ifndef MYGAME_GAMESTATE_H
#define MYGAME_GAMESTATE_H
#include "../audio/SoundManager.h"

class GameState {
public:
    explicit GameState(SoundManager& soundManager)
    : state(IN_MENU_GAME_NOT_STARTED), settingState(CLOSE), gameEditModeState(DISABLED), soundManager(soundManager) {
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
    void toggleSettingsState() {
        settingState = (settingState == OPEN) ? CLOSE : OPEN;
    }
    void toggleGameEditMode() {
        gameEditModeState = (gameEditModeState == ENABLED) ? DISABLED : ENABLED;
    }
    void setGameEditMode(bool enabled) {
        gameEditModeState = enabled ? ENABLED : DISABLED;
    }
    bool isInMenuAndGameDidNotStart() const {return state == IN_MENU_GAME_NOT_STARTED;}
    bool isInMenuAndGameAlreadyStarted() const {return state == IN_MENU_GAME_ALREADY_STARTED;}
    bool isSettingOpen() const {return settingState == OPEN;}
    bool isSettingClose() const {return settingState == CLOSE;}
    bool isGameEditModeEnabled() const {return gameEditModeState == ENABLED;}
    bool isGameEditModeDisabled() const {return gameEditModeState == DISABLED;}

private:
    enum State {
        IN_MENU_GAME_NOT_STARTED,
        IN_MENU_GAME_ALREADY_STARTED,
        IN_GAME,
    };
    enum SettingsState {
        OPEN,
        CLOSE
    };
    enum GameEditModeState {
        ENABLED,
        DISABLED
    };
    State state;
    SettingsState settingState;
    GameEditModeState gameEditModeState;
    SoundManager& soundManager;
};


#endif //MYGAME_GAMESTATE_H