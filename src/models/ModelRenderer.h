

#ifndef MODELRENDERER_H
#define MODELRENDERER_H
#include "Model.h"
#include "../camera/camera.h"
#include "../shaders/shader.h"


class ModelRenderer {
private:
    Shader* shader;
    Camera* camera;
public:
    ModelRenderer(Camera* camera);
    ~ModelRenderer() = default;

    void render(Model model);
};



#endif //MODELRENDERER_H
