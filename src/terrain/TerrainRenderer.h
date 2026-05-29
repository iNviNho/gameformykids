#ifndef TERRAINRENDERER_H
#define TERRAINRENDERER_H
#include "Terrain.h"
#include "../camera/camera.h"
#include "../models/EntityRenderer.h"
#include "../shaders/shader.h"
#include <optional>

class TerrainRenderer {
private:
    Camera& camera;
    EntityRenderer& entityRenderer;
    Shader shader;
    Screen& screen;

    bool renderEditTerrainCircle = false;
public:
    TerrainRenderer(Camera& camera, EntityRenderer& entityRenderer, Screen& screen);
    ~TerrainRenderer() = default;

    void render(Terrain& terrain, const std::optional<glm::vec3> mouseCoord, const float renderEditTerrainCircleRadius);
    void SetRenderEditTerrainCircle(const bool value) {
        renderEditTerrainCircle = value;
    }
};



#endif //TERRAINRENDERER_H
