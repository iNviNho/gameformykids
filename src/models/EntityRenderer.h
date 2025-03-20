

#ifndef MODELRENDERER_H
#define MODELRENDERER_H
#include "../camera/camera.h"
#include "../shaders/shader.h"
#include "../terrain/EntitiesHolder.h"


class EntityRenderer {
private:
    Shader* singleInstanceShader;
    Shader* multiInstanceShader;
    Camera* camera;
public:
    explicit EntityRenderer(Camera* camera);
    ~EntityRenderer() = default;

    void render(Entity* entity);
    void renderBatch(const EntitiesHolder& modelsHolder);
};



#endif //MODELRENDERER_H
