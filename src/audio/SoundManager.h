#ifndef MYGAME_SOUNDMANAGER_H
#define MYGAME_SOUNDMANAGER_H

#include "miniaudio.h"
#include <map>
#include <memory>
#include <stdexcept>

class SoundManager {
public:
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;
    enum SongName {
        MENU_INTRO,
        GAME_ELWYNN
    };
    SoundManager()
    : result(), engine(), currentSongName(MENU_INTRO) {
        result = ma_engine_init(NULL, &engine);
        if (result != MA_SUCCESS) {
            throw std::runtime_error("Failed to initialize sound engine");
        }
        loadSounds();
    }
    ~SoundManager() {
        ma_engine_uninit(&engine);
        for (auto& pair: sounds) {
            ma_sound_uninit(pair.second.get());
        }
    }

    void playSound(SongName songName, bool rewindCurrent = true);

private:
    ma_result result;
    ma_engine engine;

    std::map<SongName, std::unique_ptr<ma_sound>> sounds;
    SongName currentSongName;

    void loadSounds();
    void loadSound(const std::string &soundPath, SongName songName);
};


#endif //MYGAME_SOUNDMANAGER_H