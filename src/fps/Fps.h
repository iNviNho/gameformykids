//
// Created by Vladimír Vráb on 07.02.25.
//

#ifndef FPS_H
#define FPS_H



class Fps {
private:
    int frameCount;
    float fps;
    float lastTime;
public:
    Fps();
    ~Fps() = default;
    void tick();
    float getFps() const { return fps; }
};



#endif //FPS_H
