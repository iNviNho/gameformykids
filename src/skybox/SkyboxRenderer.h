

#ifndef SKYBOXRENDERER_H
#define SKYBOXRENDERER_H
#include <filesystem>
#include "Skybox.h"
#include "../camera/camera.h"
#include "../shaders/shader.h"
#include <data_dir.h>

#include "../ui/Screen.h"

using path = std::filesystem::path;

class SkyboxRenderer {
private:
    Camera& camera;
    Shader shader;
    Screen& screen;
public:
    explicit SkyboxRenderer(Camera& camera, Screen& screen):
    shader(Shader{
        data_dir() /= path("src/shaders/files/skyboxShader.vs"),
        data_dir() /= path("src/shaders/files/skyboxShader.fs")
    }),
    camera(camera),
    screen(screen) {}

    ~SkyboxRenderer() = default;
    const void render(Skybox& skybox);
};



#endif //SKYBOXRENDERER_H
