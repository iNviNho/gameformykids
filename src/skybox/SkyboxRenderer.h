

#ifndef SKYBOXRENDERER_H
#define SKYBOXRENDERER_H
#include "Skybox.h"
#include "../camera/camera.h"
#include "../shaders/shader.h"


class SkyboxRenderer {
private:
    Shader shader;
    Camera& camera;
public:
    explicit SkyboxRenderer(Camera& camera) : shader(
    "/Users/vladino/CLionProjects/mygame/src/shaders/files/skyboxShader.vs",
    "/Users/vladino/CLionProjects/mygame/src/shaders/files/skyboxShader.fs"
    ), camera(camera) {}
    ~SkyboxRenderer() = default;
    const void render(Skybox& skybox);
};



#endif //SKYBOXRENDERER_H
