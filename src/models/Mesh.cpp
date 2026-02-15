

#include "Mesh.h"

#include "VertexBoneData.h"
#include "../utils/OpenGlErrorChecker.h"

Mesh::Mesh(
    std::vector<Vertex>&& vertices,
    std::vector<unsigned int>&& indices,
    std::vector<Texture>&& textures,
    std::vector<VertexBoneData>&& verticesWithBoneData
):
    vertices(std::move(vertices)),
    indices(std::move(indices)),
    textures(std::move(textures)),
    verticesWithBoneData(std::move(verticesWithBoneData)),
    hasBones(false) {
    setupMesh();
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    OpenGlErrorChecker::checkGLError("Mesh::setupMesh - after VAO/VBO generation");

    // Setup vertex data (VBO)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    OpenGlErrorChecker::checkGLError("Mesh::setupMesh - after VBO setup");

    // Setup indices (EBO)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    OpenGlErrorChecker::checkGLError("Mesh::setupMesh - after EBO setup");

    // Configure vertex attributes (VBO must be bound)
    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    OpenGlErrorChecker::checkGLError("Mesh::setupMesh - after vertex attributes setup");

    // Setup bone data conditionally
    hasBones = !verticesWithBoneData.empty();
    if (hasBones) {
        glGenBuffers(1, &VBOB);
        glBindBuffer(GL_ARRAY_BUFFER, VBOB);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verticesWithBoneData[0]) * verticesWithBoneData.size(), &verticesWithBoneData[0], GL_STATIC_DRAW);

        // bones & weights
        glEnableVertexAttribArray(3);
        glVertexAttribIPointer(3, MAX_NUM_BONES_PER_VERTEX, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, MAX_NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData),
                              (const GLvoid*)(MAX_NUM_BONES_PER_VERTEX * sizeof(int32_t)));
        OpenGlErrorChecker::checkGLError("Mesh::setupMesh - after bone buffer setup");
    }

    glBindVertexArray(0);
    OpenGlErrorChecker::checkGLError("Mesh::setupMesh - final");
}

void Mesh::Draw(Shader& shader) const
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // activate texture unit first
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textures.at(i).type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++);
        shader.setFloat(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures.at(i).id);
    }
    // TODO: create sun class and move it there
    shader.setVec3("light.ambient", 0.45f, 0.45f, 0.45f);
    shader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
    shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    shader.setVec3("light.position", glm::vec3(0.0f, 0.0f, -2000.0f));

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::activateTextures(Shader& shader) const {
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // activate texture unit first
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textures.at(i).type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++);
        shader.setFloat(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures.at(i).id);
    }
}

void Mesh::bindVAO() const {
    glBindVertexArray(VAO);
}