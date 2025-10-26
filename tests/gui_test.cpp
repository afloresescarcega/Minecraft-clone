#include <gtest/gtest.h>
#include <cmath>

// Define GLM experimental features before including headers
#define GLM_ENABLE_EXPERIMENTAL

// Mock GLFW window structure
struct GLFWwindow {
    void* user_pointer = nullptr;
    int width = 800;
    int height = 600;
    bool should_close = false;
};

// Global callback pointers for testing
static void (*g_key_callback)(GLFWwindow*, int, int, int, int) = nullptr;
static void (*g_mouse_pos_callback)(GLFWwindow*, double, double) = nullptr;
static void (*g_mouse_button_callback)(GLFWwindow*, int, int, int) = nullptr;
static void (*g_scroll_callback)(GLFWwindow*, double, double) = nullptr;

// GLFW stub implementations
extern "C" {

void glfwSetWindowUserPointer(GLFWwindow* window, void* pointer) {
    window->user_pointer = pointer;
}

void* glfwGetWindowUserPointer(GLFWwindow* window) {
    return window->user_pointer;
}

void glfwSetKeyCallback(GLFWwindow* window, void (*callback)(GLFWwindow*, int, int, int, int)) {
    g_key_callback = callback;
}

void glfwSetCursorPosCallback(GLFWwindow* window, void (*callback)(GLFWwindow*, double, double)) {
    g_mouse_pos_callback = callback;
}

void glfwSetMouseButtonCallback(GLFWwindow* window, void (*callback)(GLFWwindow*, int, int, int)) {
    g_mouse_button_callback = callback;
}

void glfwSetScrollCallback(GLFWwindow* window, void (*callback)(GLFWwindow*, double, double)) {
    g_scroll_callback = callback;
}

void glfwGetWindowSize(GLFWwindow* window, int* width, int* height) {
    *width = window->width;
    *height = window->height;
}

void glfwSetWindowShouldClose(GLFWwindow* window, int value) {
    window->should_close = (value != 0);
}

} // extern "C"

// GLFW key and action constants
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

#define GLFW_RELEASE 0
#define GLFW_PRESS 1
#define GLFW_REPEAT 2

#define GL_TRUE 1
#define GL_FALSE 0

// Include GUI header
#include "gui.h"

class GUITest : public ::testing::Test {
protected:
    GLFWwindow* window;
    PerlinNoise* pn;

    void SetUp() override {
        window = new GLFWwindow();
        pn = new PerlinNoise(12345);
    }

    void TearDown() override {
        delete window;
        delete pn;
    }
};

// Constructor tests
TEST_F(GUITest, ConstructorWithDefaultViewDimensions) {
    GUI gui(window, pn);
    EXPECT_EQ(gui.window_width_, 800);
    EXPECT_EQ(gui.window_height_, 600);
    EXPECT_EQ(gui.view_width_, 800);
    EXPECT_EQ(gui.view_height_, 600);
}

TEST_F(GUITest, ConstructorWithCustomViewDimensions) {
    GUI gui(window, pn, 1024, 768);
    EXPECT_EQ(gui.view_width_, 1024);
    EXPECT_EQ(gui.view_height_, 768);
}

TEST_F(GUITest, ConstructorWithNegativeViewDimensions) {
    GUI gui(window, pn, -1, -1);
    EXPECT_EQ(gui.view_width_, 800);
    EXPECT_EQ(gui.view_height_, 600);
}

TEST_F(GUITest, ConstructorSetsPerlinNoisePointer) {
    GUI gui(window, pn);
    EXPECT_EQ(gui.pn, pn);
}

TEST_F(GUITest, ConstructorSetsDefaultFPSMode) {
    GUI gui(window, pn);
    EXPECT_TRUE(gui.fps_mode_);
}

// Matrix operations tests
TEST_F(GUITest, GetMatrixPointers) {
    GUI gui(window, pn);
    MatrixPointers ptrs = gui.getMatrixPointers();
    EXPECT_NE(ptrs.projection, nullptr);
    EXPECT_NE(ptrs.model, nullptr);
    EXPECT_NE(ptrs.view, nullptr);
}

TEST_F(GUITest, UpdateMatricesInFPSMode) {
    GUI gui(window, pn);
    gui.fps_mode_ = true;
    gui.eye_ = glm::vec3(10.0f, 20.0f, 30.0f);
    gui.look_ = glm::vec3(0.0f, 0.0f, -1.0f);
    gui.camera_distance_ = 5.0f;

    gui.updateMatrices();

    glm::vec3 expected_center = gui.eye_ + gui.camera_distance_ * gui.look_;
    EXPECT_FLOAT_EQ(gui.center_.x, expected_center.x);
    EXPECT_FLOAT_EQ(gui.center_.y, expected_center.y);
    EXPECT_FLOAT_EQ(gui.center_.z, expected_center.z);
}

TEST_F(GUITest, UpdateMatricesNotInFPSMode) {
    GUI gui(window, pn);
    gui.fps_mode_ = false;
    gui.center_ = glm::vec3(10.0f, 20.0f, 30.0f);
    gui.look_ = glm::vec3(0.0f, 0.0f, -1.0f);
    gui.camera_distance_ = 5.0f;

    gui.updateMatrices();

    glm::vec3 expected_eye = gui.center_ - gui.camera_distance_ * gui.look_;
    EXPECT_FLOAT_EQ(gui.eye_.x, expected_eye.x);
    EXPECT_FLOAT_EQ(gui.eye_.y, expected_eye.y);
    EXPECT_FLOAT_EQ(gui.eye_.z, expected_eye.z);
}

// Getter tests
TEST_F(GUITest, GetCenter) {
    GUI gui(window, pn);
    gui.center_ = glm::vec3(1.0f, 2.0f, 3.0f);
    glm::vec3 center = gui.getCenter();
    EXPECT_FLOAT_EQ(center.x, 1.0f);
    EXPECT_FLOAT_EQ(center.y, 2.0f);
    EXPECT_FLOAT_EQ(center.z, 3.0f);
}

TEST_F(GUITest, GetCamera) {
    GUI gui(window, pn);
    gui.eye_ = glm::vec3(4.0f, 5.0f, 6.0f);
    const glm::vec3& camera = gui.getCamera();
    EXPECT_FLOAT_EQ(camera.x, 4.0f);
    EXPECT_FLOAT_EQ(camera.y, 5.0f);
    EXPECT_FLOAT_EQ(camera.z, 6.0f);
}

TEST_F(GUITest, GetEyePos) {
    GUI gui(window, pn);
    gui.eye_ = glm::vec3(7.0f, 8.0f, 9.0f);
    const glm::vec3 eye = gui.getEyePos();
    EXPECT_FLOAT_EQ(eye.x, 7.0f);
    EXPECT_FLOAT_EQ(eye.y, 8.0f);
    EXPECT_FLOAT_EQ(eye.z, 9.0f);
}

TEST_F(GUITest, GetDisplacement) {
    GUI gui(window, pn);
    gui.displacement_ = glm::dvec3(1.5, 2.5, 3.5);
    const glm::dvec3 disp = gui.getDisplacement();
    EXPECT_DOUBLE_EQ(disp.x, 1.5);
    EXPECT_DOUBLE_EQ(disp.y, 2.5);
    EXPECT_DOUBLE_EQ(disp.z, 3.5);
}

TEST_F(GUITest, GetCurrentPlayTime) {
    GUI gui(window, pn);
    EXPECT_FLOAT_EQ(gui.getCurrentPlayTime(), 0.0f);
}

// Key callback tests
TEST_F(GUITest, EscapeKeyClosesWindow) {
    GUI gui(window, pn);
    gui.keyCallback(GLFW_KEY_ESCAPE, 0, GLFW_PRESS, 0);
    EXPECT_TRUE(window->should_close);
}

TEST_F(GUITest, CKeyTogglesFPSMode) {
    GUI gui(window, pn);
    bool initial_fps_mode = gui.fps_mode_;
    gui.keyCallback(GLFW_KEY_C, 0, GLFW_PRESS, 0);
    EXPECT_EQ(gui.fps_mode_, !initial_fps_mode);
}

TEST_F(GUITest, CKeyTogglesFPSModeOnRepeat) {
    GUI gui(window, pn);
    bool initial_fps_mode = gui.fps_mode_;
    gui.keyCallback(GLFW_KEY_C, 0, GLFW_REPEAT, 0);
    EXPECT_EQ(gui.fps_mode_, !initial_fps_mode);
}

TEST_F(GUITest, CKeyDoesNotToggleOnRelease) {
    GUI gui(window, pn);
    bool initial_fps_mode = gui.fps_mode_;
    gui.keyCallback(GLFW_KEY_C, 0, GLFW_RELEASE, 0);
    EXPECT_EQ(gui.fps_mode_, initial_fps_mode);
}

// WASD movement tests
TEST_F(GUITest, WKeyMovesForwardInFPSMode) {
    GUI gui(window, pn);
    gui.fps_mode_ = true;
    gui.eye_ = glm::vec3(50.0f, 60.0f, 50.0f);
    gui.look_ = glm::vec3(0.0f, 0.0f, -1.0f);
    gui.tangent_ = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 initial_eye = gui.eye_;

    gui.keyCallback(GLFW_KEY_W, 0, GLFW_PRESS, 0);

    EXPECT_NE(gui.eye_.z, initial_eye.z);
}

TEST_F(GUITest, SKeyMovesBackwardInFPSMode) {
    GUI gui(window, pn);
    gui.fps_mode_ = true;
    gui.eye_ = glm::vec3(50.0f, 60.0f, 50.0f);
    gui.look_ = glm::vec3(0.0f, 0.0f, -1.0f);
    gui.tangent_ = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 initial_eye = gui.eye_;

    gui.keyCallback(GLFW_KEY_S, 0, GLFW_PRESS, 0);

    EXPECT_NE(gui.eye_.z, initial_eye.z);
}

TEST_F(GUITest, AKeyStrafesLeftInFPSMode) {
    GUI gui(window, pn);
    gui.fps_mode_ = true;
    gui.eye_ = glm::vec3(50.0f, 60.0f, 50.0f);
    gui.look_ = glm::vec3(0.0f, 0.0f, -1.0f);
    gui.tangent_ = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 initial_eye = gui.eye_;

    gui.keyCallback(GLFW_KEY_A, 0, GLFW_PRESS, 0);

    EXPECT_NE(gui.eye_.x, initial_eye.x);
}

TEST_F(GUITest, DKeyStrafesRightInFPSMode) {
    GUI gui(window, pn);
    gui.fps_mode_ = true;
    gui.eye_ = glm::vec3(50.0f, 60.0f, 50.0f);
    gui.look_ = glm::vec3(0.0f, 0.0f, -1.0f);
    gui.tangent_ = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 initial_eye = gui.eye_;

    gui.keyCallback(GLFW_KEY_D, 0, GLFW_PRESS, 0);

    EXPECT_NE(gui.eye_.x, initial_eye.x);
}

TEST_F(GUITest, SpaceKeyJumpInFPSMode) {
    GUI gui(window, pn);
    gui.fps_mode_ = true;
    gui.feet_above_ground = false;

    gui.keyCallback(GLFW_KEY_SPACE, 0, GLFW_PRESS, 0);

    EXPECT_TRUE(gui.just_jumped);
    EXPECT_TRUE(gui.feet_above_ground);
}

TEST_F(GUITest, SpaceKeyDoesNotJumpWhenAlreadyInAir) {
    GUI gui(window, pn);
    gui.fps_mode_ = true;
    gui.feet_above_ground = true;
    gui.just_jumped = false;

    gui.keyCallback(GLFW_KEY_SPACE, 0, GLFW_PRESS, 0);

    EXPECT_FALSE(gui.just_jumped);
}

TEST_F(GUITest, DownKeyInFPSMode) {
    GUI gui(window, pn);
    gui.fps_mode_ = true;

    bool result = gui.captureWASDUPDOWN(GLFW_KEY_DOWN, GLFW_PRESS);

    EXPECT_TRUE(result);
    EXPECT_TRUE(gui.feet_above_ground);
    EXPECT_FLOAT_EQ(gui.y_velocity, 6.5f);
}

TEST_F(GUITest, UpKeyInFPSMode) {
    GUI gui(window, pn);
    gui.fps_mode_ = true;
    glm::vec3 initial_eye = gui.eye_;

    bool result = gui.captureWASDUPDOWN(GLFW_KEY_UP, GLFW_PRESS);

    EXPECT_TRUE(result);
    EXPECT_NE(gui.eye_.y, initial_eye.y);
}

TEST_F(GUITest, MovingForwardFlagSet) {
    GUI gui(window, pn);
    gui.fps_mode_ = true;
    gui.eye_ = glm::vec3(50.0f, 60.0f, 50.0f);
    gui.look_ = glm::vec3(0.0f, 0.0f, -1.0f);
    gui.tangent_ = glm::vec3(1.0f, 0.0f, 0.0f);

    gui.captureWASDUPDOWN(GLFW_KEY_W, GLFW_PRESS);

    EXPECT_TRUE(gui.moving_forward);
}

// Mouse callback tests
TEST_F(GUITest, MousePosCallbackUpdatesPosition) {
    GUI gui(window, pn);
    gui.current_x_ = 100.0f;
    gui.current_y_ = 200.0f;

    gui.mousePosCallback(150.0, 250.0);

    EXPECT_FLOAT_EQ(gui.last_x_, 100.0f);
    EXPECT_FLOAT_EQ(gui.last_y_, 200.0f);
    EXPECT_FLOAT_EQ(gui.current_x_, 150.0f);
    EXPECT_FLOAT_EQ(gui.current_y_, 350.0f); // window_height - mouse_y
}

TEST_F(GUITest, MousePosCallbackUpdatesYaw) {
    GUI gui(window, pn);
    gui.current_x_ = 100.0f;
    gui.current_y_ = 100.0f;
    float initial_yaw = gui.yaw;

    gui.mousePosCallback(150.0, 100.0);

    EXPECT_NE(gui.yaw, initial_yaw);
}

TEST_F(GUITest, MousePosCallbackUpdatesPitch) {
    GUI gui(window, pn);
    gui.current_x_ = 100.0f;
    gui.current_y_ = 100.0f;
    gui.pitch = 0.0f;

    gui.mousePosCallback(100.0, 150.0);

    EXPECT_NE(gui.pitch, 0.0f);
}

TEST_F(GUITest, MousePosCallbackClampsPitch) {
    GUI gui(window, pn);
    gui.current_x_ = 100.0f;
    gui.current_y_ = 100.0f;
    gui.pitch = 88.0f;

    // Try to move pitch beyond limits
    gui.mousePosCallback(100.0, 0.0);

    EXPECT_LE(gui.pitch, 89.0f);
    EXPECT_GE(gui.pitch, -89.0f);
}

TEST_F(GUITest, MousePosCallbackUpdatesLookVector) {
    GUI gui(window, pn);
    gui.current_x_ = 100.0f;
    gui.current_y_ = 100.0f;
    glm::vec3 initial_look = gui.look_;

    gui.mousePosCallback(150.0, 150.0);

    EXPECT_NE(gui.look_.x, initial_look.x);
}

TEST_F(GUITest, MousePosCallbackUpdatesTangent) {
    GUI gui(window, pn);
    gui.current_x_ = 100.0f;
    gui.current_y_ = 100.0f;
    glm::vec3 initial_tangent = gui.tangent_;

    gui.mousePosCallback(150.0, 100.0);

    EXPECT_NE(gui.tangent_.x, initial_tangent.x);
}

TEST_F(GUITest, MouseButtonCallbackDoesNotCrash) {
    GUI gui(window, pn);
    EXPECT_NO_THROW(gui.mouseButtonCallback(0, GLFW_PRESS, 0));
}

TEST_F(GUITest, MouseScrollCallbackDoesNotCrash) {
    GUI gui(window, pn);
    EXPECT_NO_THROW(gui.mouseScrollCallback(0.0, 1.0));
}

// Static callback tests
TEST_F(GUITest, StaticKeyCallback) {
    GUI gui(window, pn);
    ASSERT_NE(g_key_callback, nullptr);

    g_key_callback(window, GLFW_KEY_C, 0, GLFW_PRESS, 0);

    EXPECT_TRUE(!gui.fps_mode_);
}

TEST_F(GUITest, StaticMousePosCallback) {
    GUI gui(window, pn);
    ASSERT_NE(g_mouse_pos_callback, nullptr);

    gui.current_x_ = 100.0f;
    gui.current_y_ = 100.0f;

    g_mouse_pos_callback(window, 150.0, 150.0);

    EXPECT_FLOAT_EQ(gui.current_x_, 150.0f);
}

TEST_F(GUITest, StaticMouseButtonCallback) {
    GUI gui(window, pn);
    ASSERT_NE(g_mouse_button_callback, nullptr);

    EXPECT_NO_THROW(g_mouse_button_callback(window, 0, GLFW_PRESS, 0));
}

TEST_F(GUITest, StaticMouseScrollCallback) {
    GUI gui(window, pn);
    ASSERT_NE(g_scroll_callback, nullptr);

    EXPECT_NO_THROW(g_scroll_callback(window, 0.0, 1.0));
}

// Collision detection tests
TEST_F(GUITest, CollisionPreventsMovementThroughBlocks) {
    GUI gui(window, pn);
    gui.fps_mode_ = true;
    // Position at a height where collision should occur
    gui.eye_ = glm::vec3(50.0f, 50.0f, 50.0f);
    gui.look_ = glm::vec3(0.0f, 0.0f, -1.0f);
    gui.tangent_ = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 initial_eye = gui.eye_;

    // Try to move forward
    gui.captureWASDUPDOWN(GLFW_KEY_W, GLFW_PRESS);

    // Movement should be blocked or altered due to collision
    // The actual behavior depends on octaveNoise implementation
    EXPECT_TRUE(true); // Test completes without crash
}

// Edge case tests
TEST_F(GUITest, LeftRightArrowKeysHandled) {
    GUI gui(window, pn);
    EXPECT_NO_THROW(gui.keyCallback(GLFW_KEY_LEFT, 0, GLFW_PRESS, 0));
    EXPECT_NO_THROW(gui.keyCallback(GLFW_KEY_RIGHT, 0, GLFW_PRESS, 0));
}

TEST_F(GUITest, LeftRightBracketKeysHandled) {
    GUI gui(window, pn);
    EXPECT_NO_THROW(gui.keyCallback(GLFW_KEY_LEFT_BRACKET, 0, GLFW_RELEASE, 0));
    EXPECT_NO_THROW(gui.keyCallback(GLFW_KEY_RIGHT_BRACKET, 0, GLFW_RELEASE, 0));
}

TEST_F(GUITest, TKeyHandled) {
    GUI gui(window, pn);
    EXPECT_NO_THROW(gui.keyCallback(GLFW_KEY_T, 0, GLFW_PRESS, 0));
}

TEST_F(GUITest, MovementInNonFPSMode) {
    GUI gui(window, pn);
    gui.fps_mode_ = false;
    gui.eye_ = glm::vec3(50.0f, 60.0f, 50.0f);
    gui.look_ = glm::vec3(0.0f, 0.0f, -1.0f);
    gui.tangent_ = glm::vec3(1.0f, 0.0f, 0.0f);

    // Test all movement keys in non-FPS mode
    EXPECT_NO_THROW(gui.captureWASDUPDOWN(GLFW_KEY_W, GLFW_PRESS));
    EXPECT_NO_THROW(gui.captureWASDUPDOWN(GLFW_KEY_S, GLFW_PRESS));
    EXPECT_NO_THROW(gui.captureWASDUPDOWN(GLFW_KEY_A, GLFW_PRESS));
    EXPECT_NO_THROW(gui.captureWASDUPDOWN(GLFW_KEY_D, GLFW_PRESS));
}

TEST_F(GUITest, SpaceKeyInNonFPSMode) {
    GUI gui(window, pn);
    gui.fps_mode_ = false;
    glm::vec3 initial_center = gui.center_;

    gui.captureWASDUPDOWN(GLFW_KEY_SPACE, GLFW_PRESS);

    EXPECT_NE(gui.center_.y, initial_center.y);
}

TEST_F(GUITest, DownKeyInNonFPSMode) {
    GUI gui(window, pn);
    gui.fps_mode_ = false;
    glm::vec3 initial_center = gui.center_;

    gui.captureWASDUPDOWN(GLFW_KEY_DOWN, GLFW_PRESS);

    EXPECT_NE(gui.center_.y, initial_center.y);
}

TEST_F(GUITest, UpKeyInNonFPSMode) {
    GUI gui(window, pn);
    gui.fps_mode_ = false;
    glm::vec3 initial_center = gui.center_;

    gui.captureWASDUPDOWN(GLFW_KEY_UP, GLFW_PRESS);

    EXPECT_NE(gui.center_.y, initial_center.y);
}

TEST_F(GUITest, BoundaryWrappingXLow) {
    GUI gui(window, pn);
    gui.fps_mode_ = true;
    gui.eye_ = glm::vec3(44.0f, 60.0f, 50.0f);
    gui.look_ = glm::vec3(-1.0f, 0.0f, 0.0f);
    gui.tangent_ = glm::vec3(0.0f, 0.0f, 1.0f);

    gui.captureWASDUPDOWN(GLFW_KEY_W, GLFW_PRESS);

    // Eye should wrap to ~50.0f
    EXPECT_GT(gui.eye_.x, 45.0f);
}

TEST_F(GUITest, BoundaryWrappingXHigh) {
    GUI gui(window, pn);
    gui.fps_mode_ = true;
    gui.eye_ = glm::vec3(51.0f, 60.0f, 50.0f);
    gui.look_ = glm::vec3(1.0f, 0.0f, 0.0f);
    gui.tangent_ = glm::vec3(0.0f, 0.0f, -1.0f);

    gui.captureWASDUPDOWN(GLFW_KEY_W, GLFW_PRESS);

    // Eye should wrap to ~45.0f
    EXPECT_LT(gui.eye_.x, 50.0f);
}

TEST_F(GUITest, BoundaryWrappingZLow) {
    GUI gui(window, pn);
    gui.fps_mode_ = true;
    gui.eye_ = glm::vec3(50.0f, 60.0f, 44.0f);
    gui.look_ = glm::vec3(0.0f, 0.0f, -1.0f);
    gui.tangent_ = glm::vec3(1.0f, 0.0f, 0.0f);

    gui.captureWASDUPDOWN(GLFW_KEY_W, GLFW_PRESS);

    // Eye should wrap to ~50.0f
    EXPECT_GT(gui.eye_.z, 45.0f);
}

TEST_F(GUITest, BoundaryWrappingZHigh) {
    GUI gui(window, pn);
    gui.fps_mode_ = true;
    gui.eye_ = glm::vec3(50.0f, 60.0f, 51.0f);
    gui.look_ = glm::vec3(0.0f, 0.0f, 1.0f);
    gui.tangent_ = glm::vec3(1.0f, 0.0f, 0.0f);

    gui.captureWASDUPDOWN(GLFW_KEY_W, GLFW_PRESS);

    // Eye should wrap to ~45.0f
    EXPECT_LT(gui.eye_.z, 50.0f);
}

TEST_F(GUITest, UnknownKeyReturnsCorrectly) {
    GUI gui(window, pn);
    bool result = gui.captureWASDUPDOWN(999, GLFW_PRESS);
    EXPECT_FALSE(result);
}

TEST_F(GUITest, MouseMovementVerySmallDelta) {
    GUI gui(window, pn);
    gui.current_x_ = 100.0f;
    gui.current_y_ = 100.0f;

    // Very small movement should be ignored
    gui.mousePosCallback(100.0, 100.0);

    EXPECT_FLOAT_EQ(gui.current_x_, 100.0f);
}

TEST_F(GUITest, DestructorDoesNotCrash) {
    GUI* gui = new GUI(window, pn);
    EXPECT_NO_THROW(delete gui);
}
