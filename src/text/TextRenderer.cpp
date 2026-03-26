#include "TextRenderer.h"

#include <utility>
#include <filesystem>

#include "TextToBeRendered.h"
#include "glm/ext/matrix_clip_space.hpp"
#include <data_dir.h>

#include "Character.h"
#include "../utils/Log.h"
#include "../utils/OpenGlErrorChecker.h"

using path = std::filesystem::path;


TextRenderer::TextRenderer(const Screen& screen):
    shader(Shader{
    data_dir() /= path("src/shaders/files/textShader.vs"),
    data_dir() /= path("src/shaders/files/textShader.fs")
    }) {
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" <<std::endl;
    }
    if (FT_New_Face(
        ft,
        (data_dir() /= path("resources/fonts/Arial.ttf")).c_str(),
        0,
        &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }
    FT_Set_Pixel_Sizes(face, 0, 48);
    textUniformLocation = glGetUniformLocation(shader.ID, "textColor");
    textProjection = glm::ortho(0.0f, static_cast<float>(screen.GetWidth()), 0.0f, static_cast<float>(screen.GetHeight()));
    shader.use();
    shader.setMat4("projection", textProjection);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    FT_GlyphSlot g = face->glyph;
    int totalWidth = 0;
    int maxHeight = 0;
    for (int i = 0; i < 128; i++) {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
            Log::logError("Failed to load Glyph for global texture");
            continue;
        }

        totalWidth += g->bitmap.width;
        maxHeight = std::max<unsigned int>(maxHeight, g->bitmap.rows);
    }

    atlasHeight = maxHeight;
    atlasWidth = totalWidth;
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glGenTextures(1, &globalTexture);
    glBindTexture(GL_TEXTURE_2D, globalTexture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        atlasWidth,
        maxHeight,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    OpenGlErrorChecker::checkGLError("TextRenderer::TextRenderer - after creating texture atlas big texture");

    // this fills the texture we have created before
    for (int i = 0, xOffset = 0; i < 128; i++) {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
            Log::logError("Failed to load Glyph for global texture");
            continue;
        }

        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            xOffset,
            0,
            g->bitmap.width,
            g->bitmap.rows,
            GL_RED,
            GL_UNSIGNED_BYTE,
            g->bitmap.buffer
        );

        Character character {
            glm::ivec2(g->bitmap.width, g->bitmap.rows),
            glm::ivec2(g->bitmap_left, g->bitmap_top),
            glm::ivec2(g->advance.x >> 6, g->advance.y >> 6),
            (float) xOffset / totalWidth,
        };
        characters[i] = character;
        xOffset += g->bitmap.width;
    }

    // https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Text_Rendering_02
    // we want to create cache of characters for later use.
    OpenGlErrorChecker::checkGLError("TextRenderer::TextRenderer - after filling texture atlas with sub textures");

    // Configure VAO with VBO
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}


void TextRenderer::BufferText(const char* text, float x, float y, float scale, const glm::vec3& color) {
    textsToBeRendered.emplace_back(text, x, y, scale, color);
}

void TextRenderer::RenderBufferedText() {
    // activate corresponding render state
    shader.use();
    // temporary as all texts must share the same color
    // TODO: Fix me
    glUniform3f(textUniformLocation, 1.0f, 1.0f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, globalTexture);
    glBindVertexArray(textVAO);

    int allTextSizes = 0;
    for (auto& textToBeRendered: textsToBeRendered) {
        allTextSizes += textToBeRendered.GetText().size();
    }

    // resize the vectorOfPoints
    vectorOfPoints.resize(6 * allTextSizes);
    
    int n = 0;
    for (TextToBeRendered& textToBeRendered : textsToBeRendered) {
        // https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Text_Rendering_02
        // we iterate over each character in the text
        float x = textToBeRendered.GetX();
        float y = textToBeRendered.GetY();
        for(const char *p = textToBeRendered.GetText().c_str(); *p; p++) {
            unsigned char uc = static_cast<unsigned char>(*p);
            if (uc >= 128) continue;
            Character c = characters[uc];
            float x2 =  x + c.Bearing.x * textToBeRendered.GetScale();
            float y2 = -y - c.Bearing.y * textToBeRendered.GetScale();
            float w = c.Size.x * textToBeRendered.GetScale();
            float h = c.Size.y * textToBeRendered.GetScale();

            /* Advance the cursor to the start of the next character */
            x += c.Advance.x * textToBeRendered.GetScale();
            y += c.Advance.y * textToBeRendered.GetScale();

            /* Skip glyphs that have no pixels */
            if(!w || !h)
                continue;

            vectorOfPoints[n++] = {x2,-y2, c.xOffset, 0};
            vectorOfPoints[n++] = {x2 + w, -y2, c.xOffset + (float)c.Size.x / atlasWidth,0};
            vectorOfPoints[n++] = {x2,-y2 - h, c.xOffset,  (float) c.Size.y / atlasHeight}; //remember: each glyph occupies a different amount of vertical space
            vectorOfPoints[n++] = {x2 + w, -y2    , c.xOffset + (float)c.Size.x / atlasWidth,   0};
            vectorOfPoints[n++] = {x2,     -y2 - h, c.xOffset,(float) c.Size.y / atlasHeight};
            vectorOfPoints[n++] = {x2 + w, -y2 - h, c.xOffset + (float)c.Size.x / atlasWidth,(float) c.Size.y / atlasHeight};
        }
    }

    // one draw for the whole batch of buffered text
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point) * vectorOfPoints.size(), vectorOfPoints.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, n);

    OpenGlErrorChecker::checkGLError("TextRenderer::renderIndividualText");

    // we will unbind current VAO
    glBindVertexArray(0);
    // set texture to 0
    glBindTexture(GL_TEXTURE_2D, 0);

    // once we rendered all buffered text, we will clean the text buffer
    textsToBeRendered.clear();
}
