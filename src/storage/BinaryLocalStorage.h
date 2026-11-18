#ifndef BINARY_LOCAL_STORAGE
#define BINARY_LOCAL_STORAGE


#include <vector>
#include <cstdint>
#include <string>
#include <utility>

static_assert(sizeof(std::int32_t) == 4, "Terrain storage requires 32-bit integers");

// we will use int32_t to make sure it is stored as 4 bytes (32 bits)
struct Coordinate {
  std::int32_t x;
  std::int32_t z;
  float y;
};
static_assert(sizeof(Coordinate) == 12, "Terrain storage records must be exactly 12 bytes");

// the BinaryLocalStorage is heavily tied to storing coordinates
// once we have a need to use this class for other needs, we should
// rewrite it to use templates (K, V)
class BinaryLocalStorage {
    public:
        explicit BinaryLocalStorage(std::string filename): filename(std::move(filename)) {}

        // Updates the stored values for the supplied coordinates.
        void SetMultiple(const std::vector<Coordinate>& coordinates);
        std::vector<Coordinate> GetAll();

    private:
        std::string filename;  


};

#endif
