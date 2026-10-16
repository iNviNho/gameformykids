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
    (shader_dir() /= path("staticShader.vs")).string().c_str(),
    (shader_dir() /= path("staticShader.fs")).string().c_str()
    })
    {}

    void Render(const StaticShape& shape);
private:
    Shader shader;
};


#endif //MYGAME_STATICSHAPERENDERER_H
