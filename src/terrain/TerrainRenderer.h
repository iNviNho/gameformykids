#ifndef TERRAINRENDERER_H
#define TERRAINRENDERER_H
#include "Terrain.h"
#include "../camera/camera.h"
#include "../models/ModelRenderer.h"
#include "../shaders/shader.h"


class TerrainRenderer {
private:
    Shader* shader;
    Camera* camera;
    ModelRenderer* modelRenderer;
public:
    TerrainRenderer(Camera* camera, ModelRenderer* modelRenderer);
    ~TerrainRenderer() = default;

    void render(Terrain& terrain);
};



#endif //TERRAINRENDERER_H
