

#ifndef MODELRENDERER_H
#define MODELRENDERER_H
#include "Model.h"
#include "../camera/camera.h"
#include "../shaders/shader.h"
#include "../terrain/EntitiesHolder.h"


class ModelRenderer {
private:
    Shader* singleInstanceShader;
    Shader* multiInstanceShader;
    Camera* camera;
public:
    ModelRenderer(Camera* camera);
    ~ModelRenderer() = default;

    void render(Model model);
    void renderBatch(const EntitiesHolder& modelsHolder);
};



#endif //MODELRENDERER_H
