//
// Created by Vladimír Vráb on 06.02.25.
//

#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H
#include <glm/glm.hpp>

#include "../shaders/shader.h"
#include <ft2build.h>
#include FT_FREETYPE_H

class TextRenderer {
public:
    TextRenderer(int screenWidth, int screenHeight);
    ~TextRenderer() = default;
    void RenderBlackText(std::string text, float x, float y, float scale);
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color);
private:
    Shader* shader;
    FT_Library ft;
    FT_Face face;
    glm::mat4 textProjection;
};



#endif //TEXTRENDERER_H
