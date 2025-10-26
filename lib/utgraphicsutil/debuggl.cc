#include "debuggl.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const char *DebugGLErrorToString(const int error) {
    switch (error) {
        case GL_NO_ERROR:
            return "GL_NO_ERROR";
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
        default:
            return "Unknown Error";
    }
    return "Unknown Error";
}

void debugglTerminate() {
    glfwTerminate();
}
