#ifndef IMAGE_H
#define IMAGE_H



class Image {
private:
    unsigned char* data;
    int width, height, nrChannels;
public:
    explicit Image(char const* path);
    ~Image() = default;
    float getGrayscaleValue(int x, int y);
    bool isBlackColor(int x, int y);
    [[nodiscard]] int getWidth() const { return width; }
};



#endif //IMAGE_H
