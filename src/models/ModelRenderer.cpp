

#include "ModelRenderer.h"

ModelRenderer::ModelRenderer(Camera* camera) {
    this->camera = camera;
    this->shader = new Shader(
        "/Users/vladino/CLionProjects/mygame/src/shaders/files/modelShader.vs",
        "/Users/vladino/CLionProjects/mygame/src/shaders/files/modelShader.fs"
    );
}

void ModelRenderer::render(Model modelToDraw) {
    shader->use();

    // view/projection transformations
    // TODO: Does it always have to be generated?
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 view = camera->GetViewMatrix();
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 3.0f, -3.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    shader->setMat4("model", model);
    modelToDraw.Draw(*shader);
}
