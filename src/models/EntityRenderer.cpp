

#include "EntityRenderer.h"

#include <filesystem>
#include "../objects/Entity.h"
#include <data_dir.h>

using path = std::filesystem::path;

EntityRenderer::EntityRenderer(const Camera& camera):
    singleInstanceShader(Shader{
    data_dir() /= path("src/shaders/files/singleInstanceShader.vs"),
    data_dir() /= path("src/shaders/files/modelShader.fs")
    }),
    multiInstanceShader(Shader{
        data_dir() /= path("src/shaders/files/multiInstanceShader.vs"),
        data_dir() /= path("src/shaders/files/modelShader.fs")
    }),
    camera(camera) {}

void EntityRenderer::render(const Entity& entity) {
    singleInstanceShader.use();

    // view/projection transformations
    // TODO: Does it always have to be generated?
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
    const glm::mat4& view = camera.GetViewMatrix();
    singleInstanceShader.setMat4("projection", projection);
    singleInstanceShader.setMat4("view", view);

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, entity.GetPosition());

    model = glm::rotate(model, glm::radians(entity.GetRotationXAngle()), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(entity.GetRotationYAngle()), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(entity.GetRotationZAngle()), glm::vec3(0.0f, 0.0f, 1.0f));

    // TODO: Implement scaling
    // model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
    singleInstanceShader.setMat4("model", model);
    entity.GetModel().Draw(singleInstanceShader);
}

void EntityRenderer::renderBatch(const EntitiesHolder& modelsHolder) {

    // enable culling
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    multiInstanceShader.use();

    // view/projection transformations
    // TODO: Does it always have to be generated?
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
    const glm::mat4& view = camera.GetViewMatrix();
    multiInstanceShader.setMat4("projection", projection);
    multiInstanceShader.setMat4("view", view);

    // activate textures
    // const std::shared_ptr<Model> firstModel = modelsHolder.GetEntities()[0].GetModel();
    const Model& firstModel = modelsHolder.GetEntities().front().GetModel();
    const Mesh& firstMesh = firstModel.GetMeshes().front();
    firstMesh.activateTextures(multiInstanceShader);

    // TODO: create sun class and move it there
    multiInstanceShader.setVec3("light.ambient", 0.45f, 0.45f, 0.45f);
    multiInstanceShader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
    multiInstanceShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    multiInstanceShader.setVec3("light.position", glm::vec3(10.0f, 0.0f, 20.0f));

    for (unsigned int i = 0; i < firstModel.GetMeshes().size(); i++)
    {
        glBindVertexArray(firstModel.GetMeshes().at(i).GetVAO());
        glDrawElementsInstanced(
            GL_TRIANGLES,
            static_cast<unsigned int>(firstModel.GetMeshes().at(i).GetIndices().size()),
            GL_UNSIGNED_INT,
            0,
            modelsHolder.GetEntities().size());
        glBindVertexArray(0);
    }

    // glDisable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
}
