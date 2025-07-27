

#include "Skybox.h"

#include "../images/Image.h"
#include "../models/ModelGenerator.h"
#include "../textures/TextureLoader.h"
#include <data_dir.h>

using path = std::filesystem::path;

void Skybox::loadCubemap(const char* skyboxName) {

    // prepare faces
    std::vector<std::filesystem::path> faces;
    faces.push_back(std::move(data_dir() /= path("resources/images/skybox") /= path(skyboxName) /= path("left.png")));
    faces.push_back(std::move(data_dir() /= path("resources/images/skybox") /= path(skyboxName) /= path("right.png")));
    faces.push_back(std::move(data_dir() /= path("resources/images/skybox") /= path(skyboxName) /= path("top.png")));
    faces.push_back(std::move(data_dir() /= path("resources/images/skybox") /= path(skyboxName) /= path("bottom.png")));
    faces.push_back(std::move(data_dir() /= path("resources/images/skybox") /= path(skyboxName) /= path("front.png")));
    faces.push_back(std::move(data_dir() /= path("resources/images/skybox") /= path(skyboxName) /= path("back.png")));

    // Load cubemap texture
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    for (unsigned int i = 0; i < faces.size(); i++) {
        auto image = Image{faces.at(i)};
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
