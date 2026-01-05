#include "StaticShapeRenderer.h"

void StaticShapeRenderer::Render(const StaticShape& shape) {
    glDisable(GL_DEPTH_TEST);
    // static render background image
    shader.use();

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::scale(model, glm::vec3(shape.GetScale().x, shape.GetScale().y, 0.1f));

    shader.setMat4("model", model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shape.GetTexture());

    // draw mesh
    glBindVertexArray(shape.GetVAO());
    // 6 vertices only
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // good practice to reset
    glEnable(GL_DEPTH_TEST);
}
