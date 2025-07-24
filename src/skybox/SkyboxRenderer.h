

#ifndef SKYBOXRENDERER_H
#define SKYBOXRENDERER_H
#include <filesystem>
#include "Skybox.h"
#include "../camera/camera.h"
#include "../shaders/shader.h"
#include <data_dir.h>


class SkyboxRenderer {
private:
    using path = std::filesystem::path;
    Camera& camera;
    Shader shader;
public:
    explicit SkyboxRenderer(Camera& camera):
    shader(Shader{
        data_dir() /= path("src/shaders/files/skyboxShader.vs"),
        data_dir() /= path("src/shaders/files/skyboxShader.fs")
    }),
    camera(camera) {}

    ~SkyboxRenderer() = default;
    const void render(Skybox& skybox);
};



#endif //SKYBOXRENDERER_H
