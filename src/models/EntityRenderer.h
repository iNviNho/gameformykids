

#ifndef MODELRENDERER_H
#define MODELRENDERER_H
#include "../camera/camera.h"
#include "../shaders/shader.h"
#include "../terrain/EntitiesHolder.h"
#include "../ui/Screen.h"


class EntityRenderer {
private:
    Shader singleInstanceShader;
    Shader multiInstanceShader;
    const Camera& camera;
    const Screen& screen;
    unsigned int boneMatricesUBO = 0;
public:
    explicit EntityRenderer(const Camera& camera, const Screen& screen);
    ~EntityRenderer();

    void render(const Entity& entity, long startTimeInMs);
    void renderBatch(const EntitiesHolder& modelsHolder);
};



#endif //MODELRENDERER_H
