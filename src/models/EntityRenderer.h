

#ifndef MODELRENDERER_H
#define MODELRENDERER_H
#include "../camera/camera.h"
#include "../shaders/shader.h"
#include "../terrain/EntitiesHolder.h"


class EntityRenderer {
private:
    Shader singleInstanceShader;
    Shader multiInstanceShader;
    const Camera& camera;
public:
    explicit EntityRenderer(const Camera& camera);
    ~EntityRenderer() = default;

    void render(const Entity& entity);
    void renderBatch(const EntitiesHolder& modelsHolder);
};



#endif //MODELRENDERER_H
