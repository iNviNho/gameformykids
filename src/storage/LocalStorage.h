#ifndef MYGAME_LOCALSTORAGE_H
#define MYGAME_LOCALSTORAGE_H
#include <vector>
#include <string>


class LocalStorage {
public:

    explicit LocalStorage(const std::string& filename): filename(filename) {}

    void persist(const std::string& data);
    std::vector<std::string> GetLines();

private:
    std::string filename;
};


#endif //MYGAME_LOCALSTORAGE_H