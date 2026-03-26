#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H
#include <array>
#include <glm/glm.hpp>

#include "../shaders/shader.h"
#include <ft2build.h>
#include <vector>

#include "../ui/Screen.h"
#include "../text/TextToBeRendered.h"
#include "Character.h"
#include FT_FREETYPE_H


struct point {
            GLfloat x;
            GLfloat y;
            GLfloat s;
            GLfloat t;
};

class TextRenderer {
public:
    explicit TextRenderer(const Screen& screen);
    ~TextRenderer() = default;
    void BufferText(const char* text, float x, float y, float scale, const glm::vec3& color);
    void RenderBufferedText();
private:
    Shader shader;
    FT_Library ft;
    FT_Face face;
    glm::mat4 textProjection;
    unsigned int globalTexture;
    std::vector<TextToBeRendered> textsToBeRendered;
    std::vector<point> vectorOfPoints;
    
    int textUniformLocation;
    int atlasWidth;
    int atlasHeight;
    unsigned int textVAO, textVBO;
    std::array<Character, 128> characters;
};



#endif //TEXTRENDERER_H
