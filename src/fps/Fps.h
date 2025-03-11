#ifndef FPS_H
#define FPS_H
#include <string>


class Fps {
private:
    int frameCount;
    float fps;
    float lastTime;
    std::string fpsAsString;
public:
    Fps();
    ~Fps() = default;
    void tick();
    float getFps() const { return fps; }
    std::string getFpsAsString() const { return fpsAsString; }
};



#endif //FPS_H
