#ifndef MYGAME_TEXTPROPORTION_H
#define MYGAME_TEXTPROPORTION_H
#include <map>
#include <string>

#include "Character.h"
#include "freetype/freetype.h"
#include "glad/glad.h"


class TextProportion {
public:
    TextProportion(const std::string& text, float scale):
        text(text),
        width(0.0f),
        height(0.0f),
        scale(scale)
    {
        loadCharacters();
        calculate();
    }
    float GetWidth() const { return width; }
    float GetHeight() const { return height; }
    float GetScale() const { return scale; }
    const std::string& GetText() const { return text; }
private:
    float width, height, scale;
    std::string text;
    FT_Library ft;
    FT_Face face;
    std::map<GLchar, Character> characters;

    void loadCharacters();
    void calculate();
    float calculateTextWidth(const std::string& text, float scale);
    float calculateTextHeight(const std::string& text, float scale);
};


#endif //MYGAME_TEXTPROPORTION_H