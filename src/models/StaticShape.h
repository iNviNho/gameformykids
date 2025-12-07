#ifndef MYGAME_STATICSHAPE_H
#define MYGAME_STATICSHAPE_H
#include <string>

#include "../textures/TextureLoader.h"
#include "glm/vec2.hpp"

// TODO: Make move-only to prevent copies?
class StaticShape {
public:
    explicit StaticShape(
        const std::string& texturePath
    ) {
        generate(texturePath);
    }

    void SetScale(const glm::vec2& newScale) {
        scale = newScale;
    }

    const glm::vec2& GetScale() const {
        return scale;
    }

    const unsigned int& GetTexture() const {
        return texture;
    }
    const unsigned int& GetVAO() const {
        return VAO;
    }
private:
    unsigned int VAO{}, VBO{}, texture{};
    glm::vec2 scale{1.0f, 1.0f};
    void generate(const std::string& texturePath);
};


#endif //MYGAME_STATICSHAPE_H