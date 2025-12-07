#ifndef MYGAME_DOOADSLOADER_H
#define MYGAME_DOOADSLOADER_H
#include "EntitiesHolder.h"
#include "../storage/LocalStorage.h"


class DoodadsLoader {
public:
    static void LoadDoodads(LocalStorage& storage, EntitiesHolder& entitiesHolder);
private:
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string getPart(const std::string& str, size_t index, char delimiter = ',');
};


#endif //MYGAME_DOOADSLOADER_H