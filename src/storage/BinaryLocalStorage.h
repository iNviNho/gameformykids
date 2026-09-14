#ifndef BINARY_LOCAL_STORAGE
#define BINARY_LOCAL_STORAGE


#include <vector>

// we will use int32_t to make sure it is stored as 4 bytes (32 bits)
struct Coordinate {
  std::int32_t x;
  std::int32_t z;
  float y;
};

// the BinaryLocalStorage is heavily tied to storing coordinates
// once we have a need to use this class for other needs, we should
// rewrite it to use templates (K, V)
class BinaryLocalStorage {
    public:
        explicit BinaryLocalStorage(std::string filename): filename(std::move(filename)) {}

        void SetMultiple(const std::vector<Coordinate>& coordinates);
        std::vector<Coordinate> GetAll();

    private:
        std::string filename;  


};

#endif
