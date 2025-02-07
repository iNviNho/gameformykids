//
// Created by Vladimír Vráb on 07.02.25.
//

#ifndef TERRAINRENDERER_H
#define TERRAINRENDERER_H
#include "Terrain.h"
#include "../camera/camera.h"
#include "../shaders/shader.h"


class TerrainRenderer {
private:
    Shader* shader;
    Camera* camera;
public:
    TerrainRenderer(Camera* camera);
    ~TerrainRenderer() = default;

    void render(Terrain terrain);
};



#endif //TERRAINRENDERER_H
