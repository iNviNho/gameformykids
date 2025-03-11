#include "Fps.h"

#include <iostream>
#include <string>

#include "GLFW/glfw3.h"

Fps::Fps() {
    frameCount = 0;
    fps = 0.0f;
    lastTime = 0.0f;
    fpsAsString = "fps: -";
}

void Fps::tick() {
    float currentFrame = glfwGetTime();

    frameCount++;
    if (currentFrame - lastTime >= 1.0f) {
        fps = frameCount / (currentFrame - lastTime);
        frameCount = 0;
        lastTime = currentFrame;
        fpsAsString = "fps: " + std::to_string(fps);
        std::cout << fpsAsString << std::endl;
    }
}
