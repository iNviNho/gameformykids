#include "TerrainRenderer.h"

#include <filesystem>
#include "../models/EntityRenderer.h"
#include <data_dir.h>

using path = std::filesystem::path;

TerrainRenderer::TerrainRenderer(Camera& camera, EntityRenderer& entityRenderer):
    camera(camera),
    entityRenderer(entityRenderer),
    shader(Shader{
        data_dir() /= path("src/shaders/files/terrainShader.vs"),
        data_dir() /= path("src/shaders/files/terrainShader.fs")
    })
{}

void TerrainRenderer::render(Terrain& terrain) {
    shader.use();

    // TODO: Does it always have to be generated?
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
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

    // enable culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glBindVertexArray(terrain.GetVAO());
    glDrawArrays(GL_TRIANGLES, 0, terrain.GetCountOfVertices());

    glDisable(GL_CULL_FACE);

    // render grasses
    entityRenderer.renderBatch(terrain.GetGrasses().GetEntities());
}
