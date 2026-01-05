#ifndef MYGAME_LOCALSTORAGE_H
#define MYGAME_LOCALSTORAGE_H
#include <vector>
#include <string>
#include <glm/glm.hpp>


class LocalStorage {
public:

    explicit LocalStorage(std::string filename): filename(std::move(filename)) {}

    void persist(const std::string& data);
    std::vector<std::string> GetLines();

    // Remove persisted entries that represent an entity at the given position (exact match).
    // Returns true if at least one line was removed.
    bool RemoveEntityByPosition(const glm::vec3& pos, float epsilon = 1e-4f);

private:
    std::string filename;
};


#endif //MYGAME_LOCALSTORAGE_H