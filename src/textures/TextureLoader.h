#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <glad/glad.h>
#include <stb_image.h>
#include <filesystem>

#include "../utils/Log.h"

class TextureLoader {
public:
    static unsigned int loadTexture(const std::filesystem::path& value) {
        Log::logInfo("Loading texture: " + std::string(value));
        int width, height, nrChannels;
        unsigned char *data = stbi_load(value.c_str(), &width, &height,
        &nrChannels, 0);

        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // mipmaps
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);

        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
            GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            Log::logError("Failed to load texture: " + std::string(value));
        }
        stbi_image_free(data);

        stbi_set_flip_vertically_on_load(true);

        return texture;
    }
};

#endif //TEXTURELOADER_H
