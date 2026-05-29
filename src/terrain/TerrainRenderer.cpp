#include "TerrainRenderer.h"

#include <filesystem>
#include "../models/EntityRenderer.h"
#include <data_dir.h>
#include <optional>

using path = std::filesystem::path;

TerrainRenderer::TerrainRenderer(Camera& camera, EntityRenderer& entityRenderer, Screen& screen):
    camera(camera),
    entityRenderer(entityRenderer),
    shader(Shader{
        data_dir() /= path("src/shaders/files/terrainShader.vs"),
        data_dir() /= path("src/shaders/files/terrainShader.fs")
    }),
    screen(screen)
{}

void TerrainRenderer::render(Terrain& terrain, const std::optional<glm::vec3> mouseCoord, const float editTerrainCircleRadius) {
    shader.use();

    // TODO: Does it always have to be generated?
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(screen.GetWidth() / screen.GetHeight()), 0.1f, 100.0f);
    shader.setMat4("projection", projection);

    shader.setMat4("view", camera.GetViewMatrix());

    glm::mat4 terrainModel = glm::mat4(1.0f);
    shader.setMat4("model", terrainModel);

    terrain.activateTextures(shader);

    shader.setFloat("terrainSize", terrain.GetSize());

    // TODO: create sun class and move it there
    shader.setVec3("light.ambient", 0.45f, 0.45f, 0.45f);
    shader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
    shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    shader.setVec3("light.position", glm::vec3(40.0f, 5.0f, -10.0f));

    // Edit terrain circle on the terrain
    if (renderEditTerrainCircle) {
        if (mouseCoord.has_value()) {
            shader.setFloat("editTerrainCircleRadius", editTerrainCircleRadius);
            shader.setBool("editTerrainCircle", true);
            shader.setVec2("mouseCoord", glm::vec2(mouseCoord.value().x, mouseCoord.value().z));
        } else {
            shader.setBool("editTerrainCircle", false);
        }
    } else {
        shader.setBool("editTerrainCircle", renderEditTerrainCircle);
    } 

    // enable culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glBindVertexArray(terrain.GetVAO());
    glDrawArrays(GL_TRIANGLES, 0, terrain.GetCountOfVertices());

    glDisable(GL_CULL_FACE);
}
