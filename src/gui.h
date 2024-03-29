#ifndef SKINNING_GUI_H
#define SKINNING_GUI_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtx/quaternion.hpp>
#include <GLFW/glfw3.h>
#include "PerlinNoise.hpp"

struct Mesh;

struct MatrixPointers {
	const glm::mat4 *projection, *model, *view;
};

struct Collisions {
    glm::vec3 bMin;
    glm::vec3 bMax;

    Collisions() : bMin(0.0f, 0.0f, 0.0f), bMax(0.0f, 0.0f, 0.0f) {}
    Collisions(glm::vec3 bMin, glm::vec3 bMax) : bMin(bMin), bMax(bMax) {}
};

class GUI {
public:
	GUI(GLFWwindow*,  PerlinNoise *pn_, int view_width = -1, int view_height = -1, int preview_height = -1);
	~GUI();
	void assignMesh(Mesh*);

	void keyCallback(int key, int scancode, int action, int mods);
	void mousePosCallback(double mouse_x, double mouse_y);
	void mouseButtonCallback(int button, int action, int mods);
	void mouseScrollCallback(double dx, double dy);
	void updateMatrices();
	MatrixPointers getMatrixPointers() const;

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MousePosCallback(GLFWwindow* window, double mouse_x, double mouse_y);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void MouseScrollCallback(GLFWwindow* window, double dx, double dy);

	glm::vec3 getCenter() const { return center_; }
	const glm::vec3& getCamera() const { return eye_; }
	bool isPoseDirty() const { return pose_changed_; }
	void clearPose() { pose_changed_ = false; }
	const float* getLightPositionPtr() const { return &light_position_[0]; }
	
	int getCurrentBone() const { return current_bone_; }
	const int* getCurrentBonePointer() const { return &current_bone_; }
	bool setCurrentBone(int i);

	bool isTransparent() const { return transparent_; }
	bool isPlaying() const { return play_; }
	float getCurrentPlayTime() const;

    const float CAMERA_SPEED = .00001f;
    int window_width = 800, window_height = 600;
    float window_center_x = static_cast<float>(window_width) / 2.0;
    float window_center_y = static_cast<float>(window_height) / 2.0; 
    glm::quat camera_rot_ = glm::quat_cast(glm::mat4(1.0f));
	glm::quat last_rot = glm::quat_cast(glm::mat4(1.0f));

    const glm::vec3 getEyePos() const {
        return eye_;
    }

    const glm::dvec3 getDisplacement() const {
        return displacement_;
    }

    glm::vec3 eye_ = glm::vec3(50.0f, 62.5f, 50.0f);
    bool feet_above_ground = false;
    bool just_jumped = false;
    PerlinNoise *pn = nullptr;
    float y_velocity = 0.0f;
    bool moving_forward = false;



	GLFWwindow* window_;
	Mesh* mesh_;

	int window_width_, window_height_;
	int view_width_, view_height_;
	int preview_height_;

	bool drag_state_ = false;
	bool fps_mode_ = true;
	bool pose_changed_ = true;
	bool transparent_ = false;
	int current_bone_ = -1;
	int current_button_ = -1;
	float roll_speed_ = M_PI / 64.0f;
	float last_x_ = 0.0f, last_y_ = 0.0f, current_x_ = 0.0f, current_y_ = 0.0f;
	float camera_distance_ = 30.0;
	float pan_speed_ = 0.5f;
	float rotation_speed_ = 0.03f;
	float zoom_speed_ = 0.5f;
	float aspect_;
    float yaw;
    float pitch;
    float character_radius = .5f;

	
	glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 look_ = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 tangent_ = glm::cross(look_, up_);
	glm::vec3 center_ = eye_ - camera_distance_ * look_;
	glm::mat3 orientation_ = glm::mat3(tangent_, up_, look_);
	glm::vec4 light_position_;
    glm::dvec3 displacement_ = glm::vec3(0.0, 0.0, 0.0);

	glm::mat4 view_matrix_ = glm::lookAt(eye_, center_, up_);
	glm::mat4 projection_matrix_;
	glm::mat4 model_matrix_ = glm::mat4(1.0f);

    /* Init the collisions box with
    *  eyes at center width but top of height (1.75 units)
    *  width and depth = .5
    */
    Collisions collisions = {glm::vec3(eye_[0] - character_radius/2.0f, eye_[1] - 1.75f, 0.0f),  glm::vec3(eye_[0] + character_radius/2.0f, 0.0f, character_radius + character_radius)};


	bool captureWASDUPDOWN(int key, int action);

	bool play_ = false;
};

#endif
