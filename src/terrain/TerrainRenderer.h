#ifndef TERRAINRENDERER_H
#define TERRAINRENDERER_H
#include "Terrain.h"
#include "../camera/camera.h"
#include "../models/EntityRenderer.h"
#include "../shaders/shader.h"


class TerrainRenderer {
private:
    Camera& camera;
    EntityRenderer& entityRenderer;
    Shader shader;
    Screen& screen;
public:
    TerrainRenderer(Camera& camera, EntityRenderer& entityRenderer, Screen& screen);
    ~TerrainRenderer() = default;

    void render(Terrain& terrain);
};



#endif //TERRAINRENDERER_H
