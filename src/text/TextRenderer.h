#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H
#include <glm/glm.hpp>

#include "../shaders/shader.h"
#include <ft2build.h>

#include "../ui/Screen.h"

#include FT_FREETYPE_H

class TextRenderer {
public:
    explicit TextRenderer(const Screen& screen);
    ~TextRenderer() = default;
    void RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color);
private:
    Shader shader;
    FT_Library ft;
    FT_Face face;
    glm::mat4 textProjection;
};



#endif //TEXTRENDERER_H
