#include "TerrainRenderer.h"

TerrainRenderer::TerrainRenderer(Camera* camera) {
    this->shader = new Shader(
    "/Users/vladino/CLionProjects/mygame/src/shaders/files/terrainShader.vs",
    "/Users/vladino/CLionProjects/mygame/src/shaders/files/terrainShader.fs"
    );
    this->camera = camera;
}

void TerrainRenderer::render(Terrain terrain) {
    shader->use();

    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
    shader->setMat4("projection", projection);

    shader->setMat4("view", camera->GetViewMatrix());

    glm::mat4 terrainModel = glm::mat4(1.0f);
    shader->setMat4("model", terrainModel);

    terrain.activateTextures(shader);

    shader->setFloat("terrainSize", terrain.GetSize());

    glBindVertexArray(terrain.GetVAO());
    glDrawArrays(GL_TRIANGLES, 0, terrain.GetCountOfVertices());
}
