#include "Image.h"

#include <iostream>
#include <stb_image.h>

#include "../utils/Log.h"

Image::Image(const std::filesystem::path& path) {
    data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (!data) {
        std::cerr << "Failed to load image: " << path << std::endl;
    }
}

float Image::getGrayscaleValue(int x, int y) const {
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return 0;
    }
    int index = (y * width + x) * nrChannels;
    const float r = data[index];
    const float g = data[index + 1];
    const float b = data[index + 2];
    return 0.299f * r + 0.587f * g + 0.114f * b;
}

bool Image::isBlackColor(int x, int y) const {
    if (x < 0 || y < 0 || x >= width || y >= height) {
        Log::logWarning("Out of bounds access in isBlackColor");
        return false;
    }
    int index = (y * width + x) * nrChannels;
    const float r = data[index];
    const float g = data[index + 1];
    const float b = data[index + 2];
    return r == 0 && g == 0 && b == 0;
}

bool Image::isBlueColor(int x, int y) const {
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return false;
    }
    int index = (y * width + x) * nrChannels;
    const float r = data[index];
    const float g = data[index + 1];
    const float b = data[index + 2];
    return r == 0 && g == 0 && b == 255;
}

bool Image::isRedColor(int x, int y) const {
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return false;
    }
    int index = (y * width + x) * nrChannels;
    const float r = data[index];
    const float g = data[index + 1];
    const float b = data[index + 2];
    return r == 255 && g == 0 && b == 0;
}


