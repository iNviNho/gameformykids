#ifndef MYGAME_SCREEN_H
#define MYGAME_SCREEN_H


class Screen {
public:
    Screen(const int width, const int height) : width(width), height(height) {}
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