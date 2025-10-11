#include "TextProportion.h"

#include <map>
#include <filesystem>

#include "Character.h"
#include "data_dir.h"
#include "../utils/Log.h"
#include "freetype/freetype.h"
#include "glm/vec2.hpp"
#include "glm/detail/func_packing_simd.inl"

using path = std::filesystem::path;

void TextProportion::loadCharacters() {
    if (FT_Init_FreeType(&ft)) {
        Log::logError("ERROR::FREETYPE: Could not init FreeType Library");
        throw std::runtime_error("Failed to initialize FreeType Library");
    }
    if (FT_New_Face(
        ft,
        (data_dir() /= path("resources/fonts/Arial.ttf")).c_str(),
        0,
        &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        }
    FT_Set_Pixel_Sizes(face, 0, 48);
    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            Log::logError("ERROR::FREETYTPE: Failed to load Glyph");
            throw std::runtime_error("Failed to load Glyph");
        }
        // now store character for later use
        Character character {
            0, // not needed
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        characters.insert(std::pair<char, Character>(c, character));
    }
}

void TextProportion::calculate() {
    width = calculateTextWidth(text, scale);
    // Log::logInfo("Calculated width for text '" + text + "': " + std::to_string(width));
    height = calculateTextHeight(text, scale);
}

// In TextRenderer.cpp
float TextProportion::calculateTextWidth(const std::string& text, float scale) {
    float width = 0.0f;
    for (const char& c : text) {
        auto it = characters.find(c);
        if (it != characters.end()) {
            width += (it->second.Advance >> 6) * scale;
        }
    }
    return width;
}

float TextProportion::calculateTextHeight(const std::string& text, float scale) {
    float maxHeight = 0.0f;
    for (const char& c : text) {
        auto it = characters.find(c);
        if (it != characters.end()) {
            float height = it->second.Size.y * scale;
            if (height > maxHeight) {
                maxHeight = height;
            }
        }
    }
    return maxHeight;
}
