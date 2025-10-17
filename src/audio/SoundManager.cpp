#include "SoundManager.h"

#include "data_dir.h"

void SoundManager::loadSounds() {
    loadSound(data_dir() / "resources/sounds/menu_intro.mp3", MENU_INTRO);
    loadSound(data_dir() / "resources/sounds/game_elwynn.mp3", GAME_ELWYNN);
}

void SoundManager::loadSound(const std::string& soundPath, SongName songName) {
    auto sound = std::make_unique<ma_sound>();

    result = ma_sound_init_from_file(&engine, soundPath.c_str(), 0, NULL, NULL, sound.get());
    if (result != MA_SUCCESS) {
        throw std::runtime_error("Failed to initialize menu intro sound");
    }
    // add sound to the map
    sounds[songName] = std::move(sound);
}

void SoundManager::playSound(SongName songName, bool rewindCurrent) {
    // stop all currently playing sounds
    for (auto& [name, sound] : sounds) {
        // we stop playing sound
        ma_sound_stop(sound.get());
        // we rewind it to the beginning
        bool isCurrent = currentSongName == name;
        if (isCurrent && rewindCurrent) {
            ma_sound_seek_to_pcm_frame(sound.get(), 0);
        }
    }

    // play the requested sound
    ma_sound_start(sounds[songName].get());
    currentSongName = songName;
}

