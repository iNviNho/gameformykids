#ifndef MYGAME_STATICSHAPE_H
#define MYGAME_STATICSHAPE_H
#include <string>

#include "../textures/TextureLoader.h"

// TODO: Make move-only to prevent copies?
class StaticShape {
public:
    explicit StaticShape(
        const std::string& texturePath
    ) {
        generate(texturePath);
    }

    const unsigned int& GetTexture() const {
        return texture;
    }
    const unsigned int& GetVAO() const {
        return VAO;
    }
private:
    unsigned int VAO{}, VBO{}, texture{};
    void generate(const std::string& texturePath);
};


#endif //MYGAME_STATICSHAPE_H