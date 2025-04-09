

#include "Skybox.h"

#include "../images/Image.h"
#include "../models/ModelGenerator.h"
#include "../textures/TextureLoader.h"

void Skybox::loadCubemap(const char* skyboxName) {

    // prepare faces
    std::vector<std::string> faces;
    faces.push_back("/Users/vladino/CLionProjects/mygame/resources/images/skybox/" + std::string(skyboxName) + "/left.png");
    faces.push_back("/Users/vladino/CLionProjects/mygame/resources/images/skybox/" + std::string(skyboxName) + "/right.png");
    faces.push_back("/Users/vladino/CLionProjects/mygame/resources/images/skybox/" + std::string(skyboxName) + "/top.png");
    faces.push_back("/Users/vladino/CLionProjects/mygame/resources/images/skybox/" + std::string(skyboxName) + "/bottom.png");
    faces.push_back("/Users/vladino/CLionProjects/mygame/resources/images/skybox/" + std::string(skyboxName) + "/front.png");
    faces.push_back("/Users/vladino/CLionProjects/mygame/resources/images/skybox/" + std::string(skyboxName) + "/back.png");

    // Load cubemap texture
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    for (unsigned int i = 0; i < faces.size(); i++) {
        auto image = Image(faces[i].c_str());
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_RGB,
            image.getWidth(),
            image.getHeight(),
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            image.getData()
        );
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
    GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
    GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
    GL_CLAMP_TO_EDGE);

    std::vector<float> skyboxVertices = generateCubemapPositions();

    // skybox VAO
    unsigned int skyboxVBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, skyboxVertices.size() * sizeof(float), skyboxVertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

std::vector<float> Skybox::generateCubemapPositions() {
 return {
     // positions
     -1.0f,  1.0f, -1.0f,
     -1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f,  1.0f, -1.0f,
     -1.0f,  1.0f, -1.0f,

     -1.0f, -1.0f,  1.0f,
     -1.0f, -1.0f, -1.0f,
     -1.0f,  1.0f, -1.0f,
     -1.0f,  1.0f, -1.0f,
     -1.0f,  1.0f,  1.0f,
     -1.0f, -1.0f,  1.0f,

      1.0f, -1.0f, -1.0f,
      1.0f, -1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,
      1.0f,  1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,

     -1.0f, -1.0f,  1.0f,
     -1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,
      1.0f, -1.0f,  1.0f,
     -1.0f, -1.0f,  1.0f,

     -1.0f,  1.0f, -1.0f,
      1.0f,  1.0f, -1.0f,
      1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,
     -1.0f,  1.0f,  1.0f,
     -1.0f,  1.0f, -1.0f,

     -1.0f, -1.0f, -1.0f,
     -1.0f, -1.0f,  1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
     -1.0f, -1.0f,  1.0f,
      1.0f, -1.0f,  1.0f
   };
}
