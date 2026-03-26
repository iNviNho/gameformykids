#ifndef MYGAME_CHARACTER_H
#define MYGAME_CHARACTER_H
#include "glm/vec2.hpp"
#include "glm/ext/matrix_clip_space.hpp"

struct Character {
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    glm::ivec2   Advance;
    float xOffset;
};

#endif //MYGAME_CHARACTER_H