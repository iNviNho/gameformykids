#ifndef MYGAME_SCREEN_H
#define MYGAME_SCREEN_H


class Screen {
public:
    explicit Screen(const int width = 800, const int height = 600) : width(width), height(height) {}
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

    void Resize(int newWidth, const int newHeight) {
        width = newWidth;
        height = newHeight;
    }
private:
    int width, height;
};


#endif //MYGAME_SCREEN_H