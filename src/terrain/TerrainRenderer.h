#ifndef TERRAINRENDERER_H
#define TERRAINRENDERER_H
#include "Terrain.h"
#include "../camera/camera.h"
#include "../models/EntityRenderer.h"
#include "../shaders/shader.h"


class TerrainRenderer {
private:
    Shader* shader;
    Camera* camera;
    EntityRenderer* entityRenderer;
public:
    TerrainRenderer(Camera* camera, EntityRenderer* modelRenderer);
    ~TerrainRenderer() = default;

    void render(Terrain& terrain);
};



#endif //TERRAINRENDERER_H
