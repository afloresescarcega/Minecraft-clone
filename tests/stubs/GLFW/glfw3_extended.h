#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Window handle type
typedef struct GLFWwindow GLFWwindow;

// Key codes
#define GLFW_KEY_ESCAPE 256
#define GLFW_KEY_J 74
#define GLFW_KEY_W 87
#define GLFW_KEY_A 65
#define GLFW_KEY_S 83
#define GLFW_KEY_D 68
#define GLFW_KEY_C 67
#define GLFW_KEY_T 84
#define GLFW_KEY_LEFT 263
#define GLFW_KEY_RIGHT 262
#define GLFW_KEY_UP 265
#define GLFW_KEY_DOWN 264
#define GLFW_KEY_SPACE 32
#define GLFW_KEY_LEFT_BRACKET 91
#define GLFW_KEY_RIGHT_BRACKET 93

// Actions
#define GLFW_RELEASE 0
#define GLFW_PRESS 1
#define GLFW_REPEAT 2

// Boolean values
#define GL_TRUE 1
#define GL_FALSE 0

// Callback function types
typedef void (* GLFWkeyfun)(GLFWwindow*, int, int, int, int);
typedef void (* GLFWcursorposfun)(GLFWwindow*, double, double);
typedef void (* GLFWmousebuttonfun)(GLFWwindow*, int, int, int);
typedef void (* GLFWscrollfun)(GLFWwindow*, double, double);

// GLFW functions
void glfwTerminate();
void glfwSetWindowUserPointer(GLFWwindow* window, void* pointer);
void* glfwGetWindowUserPointer(GLFWwindow* window);
void glfwSetKeyCallback(GLFWwindow* window, GLFWkeyfun callback);
void glfwSetCursorPosCallback(GLFWwindow* window, GLFWcursorposfun callback);
void glfwSetMouseButtonCallback(GLFWwindow* window, GLFWmousebuttonfun callback);
void glfwSetScrollCallback(GLFWwindow* window, GLFWscrollfun callback);
void glfwGetWindowSize(GLFWwindow* window, int* width, int* height);
void glfwSetWindowShouldClose(GLFWwindow* window, int value);

#ifdef __cplusplus
}
#endif
