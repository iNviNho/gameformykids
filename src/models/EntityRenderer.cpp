

#include "EntityRenderer.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include "../objects/Entity.h"
#include <data_dir.h>

#include "AnimatedModel.h"
#include "GLFW/glfw3.h"

using path = std::filesystem::path;
namespace {
constexpr GLsizeiptr MAX_BONE_MATRICES = 142;
}

EntityRenderer::EntityRenderer(const Camera& camera, const Screen& screen):
    singleInstanceShader(Shader{
    shader_dir() /= path("singleInstanceShader.vs"),
    shader_dir() /= path("modelShader.fs")
    }),
    multiInstanceShader(Shader{
        shader_dir() /= path("multiInstanceShader.vs"),
        shader_dir() /= path("modelShader.fs")
    }),
    camera(camera),
    screen(screen) {
    const GLuint boneMatricesBlock = glGetUniformBlockIndex(singleInstanceShader.ID, "BoneMatrices");
    if (boneMatricesBlock != GL_INVALID_INDEX) {
        glUniformBlockBinding(singleInstanceShader.ID, boneMatricesBlock, 0);
    }

    // TODO: I don't like that this buffer is loaded in entity renderer ... This should be loaded up in AnimatedModel or AbstractModel  
    glGenBuffers(1, &boneMatricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, boneMatricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, MAX_BONE_MATRICES * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, boneMatricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

EntityRenderer::~EntityRenderer() {
    if (boneMatricesUBO != 0) {
        glDeleteBuffers(1, &boneMatricesUBO);
    }
}

void EntityRenderer::render(const Entity& entity, long startTimeInMillis) {
    singleInstanceShader.use();

    // view/projection transformations
    // TODO: Does it always have to be generated?
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(screen.GetWidth() / screen.GetHeight()), 0.1f, 362.0f);
    const glm::mat4& view = camera.GetViewMatrix();
    singleInstanceShader.setMat4("projection", projection);
    singleInstanceShader.setMat4("view", view);

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, entity.GetPosition());

    model = glm::rotate(model, glm::radians(entity.GetRotationXAngle()), glm::vec3(1.0f, 0.0f, 0.0f));
    // TODO: This should be some kind of before render rotation y angle offset 
    model = glm::rotate(model, glm::radians(entity.GetRotationYAngle() - 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(entity.GetRotationZAngle()), glm::vec3(0.0f, 0.0f, 1.0f));

    // TODO: create sun class and move it there
    // singleInstanceShader.setVec3("light.ambient", 0.0f, 0.0f, 0.0f);
    // singleInstanceShader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
    // singleInstanceShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    // singleInstanceShader.setVec3("light.position", glm::vec3(20.0f, 0.0f, -400.0f));

    model = glm::scale(model, glm::vec3(entity.GetScale()));

    long currentTimeInMillis = static_cast<long>(glfwGetTime() * 1000);
    float animationTimeInSec = static_cast<float>(currentTimeInMillis - startTimeInMillis) / 1000.0f;

    if (entity.GetModel().IsAnimated()) {
        std::vector<glm::mat4> transforms;
        auto& animatedModel = static_cast<AnimatedModel&>(entity.GetModel());
        animatedModel.GetBoneTransforms(animationTimeInSec, transforms);

        const size_t boneCount = std::min<size_t>(transforms.size(), MAX_BONE_MATRICES);
        if (boneCount > 0) {
            glBindBuffer(GL_UNIFORM_BUFFER, boneMatricesUBO);
            glBufferSubData(
                GL_UNIFORM_BUFFER,
                0,
                static_cast<GLsizeiptr>(boneCount * sizeof(glm::mat4)),
                glm::value_ptr(transforms.front())
            );
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
    }

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
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(screen.GetWidth() / screen.GetHeight()), 0.1f, 100.0f);
    const glm::mat4& view = camera.GetViewMatrix();
    multiInstanceShader.setMat4("projection", projection);
    multiInstanceShader.setMat4("view", view);

    // activate textures
    // const std::shared_ptr<Model> firstModel = modelsHolder.GetEntities()[0].GetModel();
    AbstractModel& firstModel = modelsHolder.GetEntities().front().GetModel();
    Mesh& firstMesh = firstModel.GetMeshes().front();
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
