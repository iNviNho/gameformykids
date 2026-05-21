#ifndef MYGAME_LOCALSTORAGE_H
#define MYGAME_LOCALSTORAGE_H
#include <vector>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>


class LocalStorage {
public:

    explicit LocalStorage(std::string filename): filename(std::move(filename)) {}

    // Persist value of any string (no opinionated)  
    // Key must not contain "=" as that is used as key,value delimiter
    // Key is unique, therefore 2 calls with same key but different value 
    // means only later will be persisted
    void insert(const std::string& key, const std::string& value);

    // Persists multiple values of any strings (no opinionated)
    // Key must not contain "=" as that is used as key,value delimiter
    // Key is unique, therefore 2 calls with same key but different value 
    // means only later will be persisted
    void insertMultiple(const std::unordered_map<std::string, std::string>& data);

    // Retrieves all items 
    std::unordered_map<std::string, std::string> GetAll();

    // Retrieves single item 
    std::string getOne(const std::string& key, const std::string& defaultValue = "");

    // Removes the item by key
    void remove(const std::string& key);

    // Helper method that returns an ordered map of values split by delimiter
    std::vector<std::string> split(const std::string& str, char delimiter);
private:
    std::string filename;
};


#endif //MYGAME_LOCALSTORAGE_H
