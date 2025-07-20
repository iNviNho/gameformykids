

#include "Grasses.h"

#include <glad/glad.h>

#include "../models/Model.h"
#include "../objects/Entity.h"

void Grasses::prepare() {
    std::vector<Entity> entities = grasses.GetEntities();
    if (entities.empty()) {
        return;
    }

    const std::shared_ptr<Model> model = grasses.GetEntities().front().GetModel();
    auto modelMatrices = std::make_unique<glm::mat4[]>(entities.size());

    for (int i = 0; i < entities.size(); i++) {
        // model matrix
        auto localModel = glm::mat4(1.0f);
        localModel = glm::translate(localModel, entities.at(i).GetPosition());
        modelMatrices[i] = localModel;
    }

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, entities.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    for (unsigned int i = 0; i < model->GetMeshes().size(); i++) {
        model->GetMeshes()[i].bindVAO();
        std::size_t v4s = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4*v4s, (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4*v4s, (void*)(1*v4s));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4*v4s, (void*)(2*v4s));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4*v4s, (void*)(3*v4s));
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glBindVertexArray(0);
    }
}
