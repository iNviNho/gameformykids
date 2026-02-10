#ifndef MYGAME_OPENGLERRORCHECKER_H
#define MYGAME_OPENGLERRORCHECKER_H


#include <iostream>
#include <glad/glad.h>

#include "Log.h"

class OpenGlErrorChecker {

public:
    static void checkGLError(const std::string& location) {
        GLenum err;
        bool hasError = false;
        while ((err = glGetError()) != GL_NO_ERROR) {
            hasError = true;
            std::string errorString;
            switch (err) {
                case GL_INVALID_ENUM:
                    errorString = "GL_INVALID_ENUM";
                    break;
                case GL_INVALID_VALUE:
                    errorString = "GL_INVALID_VALUE";
                    break;
                case GL_INVALID_OPERATION:
                    errorString = "GL_INVALID_OPERATION";
                    break;
                case GL_OUT_OF_MEMORY:
                    errorString = "GL_OUT_OF_MEMORY";
                    break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:
                    errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
                    break;
                default:
                    errorString = "UNKNOWN_ERROR (" + std::to_string(err) + ")";
                    break;
            }
            Log::logError("OpenGL Error at " + location + ": " + errorString);
        }
        if (hasError) {
#ifdef DEBUG
            assert(false && "OpenGL error detected");
#endif
        }
    }
};


#endif //MYGAME_OPENGLERRORCHECKER_H