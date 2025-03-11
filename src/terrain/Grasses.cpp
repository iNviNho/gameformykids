

#include "Grasses.h"

#include <glad/glad.h>

#include "../models/Model.h"
#include "../objects/Entity.h"
#include "glm/detail/func_packing_simd.inl"

void Grasses::prepare() {
    std::vector<Entity> entities = grasses.GetEntities();
    if (entities.empty()) {
        return;
    }

    Model model = grasses.GetEntities()[0].GetModel();
    glm::mat4 *modelMatrices;
    modelMatrices = new glm::mat4[entities.size()];

    for (int i = 0; i < entities.size(); i++) {
        modelMatrices[i] = entities[i].GetModelMatrix();
    }

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, entities.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    for (unsigned int i = 0; i < model.GetMeshes().size(); i++) {
        model.GetMeshes()[i].bindVAO();
        // vertex attributes
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
