#include "SkyboxRenderer.h"

#include "../shaders/shader.h"

const void SkyboxRenderer::render(Skybox& skybox) {
    glDepthMask(GL_FALSE);
    shader.use();
    shader.setInt("skybox", 0);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(screen.GetWidth() / screen.GetHeight()), 0.1f, 100.0f);
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(skybox.GetVAO());
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.GetCubemapTexture());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
}
