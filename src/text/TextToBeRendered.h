#ifndef MYGAME_TEXT_TO_BE_RENDERED_H
#define MYGAME_TEXT_TO_BE_RENDERED_H

#include <glm/vec3.hpp>
#include <string>

class TextToBeRendered {
public:
    TextToBeRendered(std::string text, float x, float y, float scale, const glm::vec3& color):
    text(std::move(text)), x(x), y(y), scale(scale), color(color) {}

    const std::string& GetText() const { return text;}
    const float GetX() const { return x; }
    const float GetY() const { return y; }
    const float GetScale() const { return scale;}
    const glm::vec3& GetColor() const {return color;}

private:
    std::string text;
    float x;
    float y;
    float scale;
    glm::vec3 color;
};



#endif
