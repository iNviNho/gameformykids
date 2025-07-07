#include "Image.h"

#include <iostream>
#include <stb_image.h>

Image::Image(const std::filesystem::path& path) {
    data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (!data) {
        std::cerr << "Failed to load image: " << path << std::endl;
    }
    if (nrChannels < 3) {
        throw std::runtime_error("Image with path: " + path.string() + " does not have enough channels: " + std::to_string(nrChannels));
    }
}

float Image::getGrayscaleValue(int x, int y) const {
    if (x < 0 || y < 0 || x > width || y > height) {
        throw std::out_of_range("Coordinates out of bounds in getGrayscaleValue method: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    }
    int index = (y * width + x) * nrChannels;
    const unsigned char r = data[index];
    const unsigned char g = data[index + 1];
    const unsigned char b = data[index + 2];
    return 0.299f * r + 0.587f * g + 0.114f * b;
}

bool Image::isBlackColor(int x, int y) const {
    if (x < 0 || y < 0 || x > width || y > height) {
        throw std::out_of_range("Coordinates out of bounds in isBlackColor method: (x: " + std::to_string(x) + ", y: " + std::to_string(y) + ")");
    }
    int index = (y * width + x) * nrChannels;
    const unsigned char r = data[index];
    const unsigned char g = data[index + 1];
    const unsigned char b = data[index + 2];
    return r == 0 && g == 0 && b == 0;
}

bool Image::isBlueColor(int x, int y) const {
    if (x < 0 || y < 0 || x > width || y > height) {
        throw std::out_of_range("Coordinates out of bounds in isBlueColor method: (x: " + std::to_string(x) + ", y: " + std::to_string(y) + ")");
    }
    int index = (y * width + x) * nrChannels;
    const unsigned char r = data[index];
    const unsigned char g = data[index + 1];
    const unsigned char b = data[index + 2];
    return r == 0 && g == 0 && b == 255;
}

bool Image::isRedColor(int x, int y) const {
    if (x < 0 || y < 0 || x > width || y > height) {
        throw std::out_of_range("Coordinates out of bounds in isRedColor method: (x: " + std::to_string(x) + ", y: " + std::to_string(y) + ")");
    }
    int index = (y * width + x) * nrChannels;
    const unsigned char r = data[index];
    const unsigned char g = data[index + 1];
    const unsigned char b = data[index + 2];
    return r == 255 && g == 0 && b == 0;
}


