#ifndef MYGAME_STATICSHAPERENDERER_H
#define MYGAME_STATICSHAPERENDERER_H
#include "data_dir.h"
#include "StaticShape.h"
#include "../shaders/shader.h"

using path = std::filesystem::path;

class StaticShapeRenderer {
public:
    explicit StaticShapeRenderer():
    shader(Shader{
    (data_dir() /= path("src/shaders/files/staticShader.vs")).string().c_str(),
    (data_dir() /= path("src/shaders/files/staticShader.fs")).string().c_str()
    })
    {}

    void Render(const StaticShape& shape);
private:
    Shader shader;
};


#endif //MYGAME_STATICSHAPERENDERER_H