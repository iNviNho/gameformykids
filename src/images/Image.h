#ifndef IMAGE_H
#define IMAGE_H

#include <filesystem>
#include <stb_image.h>


class Image {
private:
    unsigned char* data;
    int width, height, nrChannels;
public:
    explicit Image(const std::filesystem::path& path);
    ~Image() {
        if (data) {
            stbi_image_free(data);
        }
    }
    float getGrayscaleValue(int x, int y) const;
    bool isBlackColor(int x, int y) const;
    bool isBlueColor(int x, int y) const;
    bool isRedColor(int x, int y) const;
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    const unsigned char* getData() const { return data; }
};



#endif //IMAGE_H
