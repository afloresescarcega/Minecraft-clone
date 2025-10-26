#include <gtest/gtest.h>
#include <GL/glew.h>

#include "debuggl.h"

namespace {
int g_glfwTerminateCallCount = 0;
}

extern "C" void glfwTerminate() {
    ++g_glfwTerminateCallCount;
}

TEST(DebugGLErrorToStringTest, ReturnsKnownNames) {
    struct {
        GLenum code;
        const char* name;
    } kCases[] = {
        {GL_NO_ERROR, "GL_NO_ERROR"},
        {GL_INVALID_ENUM, "GL_INVALID_ENUM"},
        {GL_INVALID_VALUE, "GL_INVALID_VALUE"},
        {GL_INVALID_OPERATION, "GL_INVALID_OPERATION"},
        {GL_OUT_OF_MEMORY, "GL_OUT_OF_MEMORY"},
    };

    for (const auto& test_case : kCases) {
        EXPECT_STREQ(test_case.name, DebugGLErrorToString(static_cast<int>(test_case.code)));
    }
}

TEST(DebugGLErrorToStringTest, ReturnsUnknownErrorForUnexpectedCode) {
    EXPECT_STREQ("Unknown Error", DebugGLErrorToString(0xDEADBEEF));
}

TEST(DebugGLTerminateTest, CallsGlfwTerminate) {
    g_glfwTerminateCallCount = 0;
    debugglTerminate();
    EXPECT_EQ(1, g_glfwTerminateCallCount);
}

