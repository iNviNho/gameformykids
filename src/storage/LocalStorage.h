#ifndef MYGAME_LOCALSTORAGE_H
#define MYGAME_LOCALSTORAGE_H
#include <vector>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>


class LocalStorage {
public:

    explicit LocalStorage(std::string filename): filename(std::move(filename)) {}

    void persist(const std::string& data);
    std::vector<std::string> GetLines();

    // Read key=value configuration pairs from the file
    std::unordered_map<std::string, std::string> GetKeyValuePairs();

    // Get a single key value with a default fallback if not found
    std::string getKeyValue(const std::string& key, const std::string& defaultValue = "");

    // Remove persisted entries that represent an entity at the given position (exact match).
    // Returns true if at least one line was removed.
    bool RemoveEntityByPosition(const glm::vec3& pos, float epsilon = 1e-4f);

private:
    std::string filename;
};


#endif //MYGAME_LOCALSTORAGE_H