#include "gui.h"
#include "config.h"
#include <jpegio.h>
#include <iostream>
#include <algorithm>
#include <debuggl.h>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <math.h> // fmod, 

namespace {
	// FIXME: Implement a function that performs proper
	//        ray-cylinder intersection detection
	// TIPS: The implement is provided by the ray-tracer starter code.
}

GUI::GUI(GLFWwindow* window, int view_width, int view_height, int preview_height)
	:window_(window), preview_height_(preview_height)
{
	glfwSetWindowUserPointer(window_, this);
	glfwSetKeyCallback(window_, KeyCallback);
	glfwSetCursorPosCallback(window_, MousePosCallback);
	glfwSetMouseButtonCallback(window_, MouseButtonCallback);
	glfwSetScrollCallback(window_, MouseScrollCallback);

	glfwGetWindowSize(window_, &window_width_, &window_height_);
	if (view_width < 0 || view_height < 0) {
		view_width_ = window_width_;
		view_height_ = window_height_;
	} else {
		view_width_ = view_width;
		view_height_ = view_height;
	}
	float aspect_ = static_cast<float>(view_width_) / view_height_;
	projection_matrix_ = glm::perspective((float)(kFov * (M_PI / 180.0f)), aspect_, kNear, kFar);
}

GUI::~GUI()
{
}

// void GUI::assignMesh(Mesh* mesh)
// {
// 	mesh_ = mesh;
// 	center_ = mesh_->getCenter();
// }

void GUI::keyCallback(int key, int scancode, int action, int mods)
{
#if 0
	if (action != 2)
		std::cerr << "Key: " << key << " action: " << action << std::endl;
#endif
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window_, GL_TRUE);
		return ;
	}
	if (key == GLFW_KEY_J && action == GLFW_RELEASE) {
		//FIXME save out a screenshot using SaveJPEG
	}
	// if (key == GLFW_KEY_S && (mods & GLFW_MOD_CONTROL)) {
	// 	if (action == GLFW_RELEASE)
	// 		mesh_->saveAnimationTo("animation.json");
	// 	return ;
	// }

	if (mods == 0 && captureWASDUPDOWN(key, action))
		return ;
	if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
		// float roll_speed;
		// if (key == GLFW_KEY_RIGHT)
		// 	roll_speed = -roll_speed_;
		// else
		// 	roll_speed = roll_speed_;
		// FIXME: actually roll the bone here
	} else if (key == GLFW_KEY_C && action != GLFW_RELEASE) {
		fps_mode_ = !fps_mode_;
	} else if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_RELEASE) {
		// current_bone_--;
		// current_bone_ += mesh_->getNumberOfBones();
		// current_bone_ %= mesh_->getNumberOfBones();
	} else if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_RELEASE) {
		// current_bone_++;
		// current_bone_ += mesh_->getNumberOfBones();
		// current_bone_ %= mesh_->getNumberOfBones();
	} else if (key == GLFW_KEY_T && action != GLFW_RELEASE) {
		// transparent_ = !transparent_;
	}

	// FIXME: implement other controls here.
}

void GUI::mousePosCallback(double mouse_x, double mouse_y)
{
    // if(last_x_ == 0.0f && last_y_ == 0.0f){
    //     last_x_= mouse_x;
    //     last_y_ = mouse_y;
    // } else {
    //     last_x_ = current_x_;
    //     last_y_ = current_y_;
    // }
	
	// current_x_ = mouse_x;
	// current_y_ = window_height_ - mouse_y;
	// float delta_x = current_x_ - last_x_;
	// float delta_y = current_y_ - last_y_;

    // yaw -= CAMERA_SPEED * delta_x ;
    // // pitch += CAMERA_SPEED * 96 * delta_y;
    // pitch -= CAMERA_SPEED * delta_y;


    // // update look with the translation

    // glm::quat rotPitch = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    // glm::quat rotYaw = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));

    // glm::quat orientation = rotPitch * rotYaw;
    // orientation = glm::normalize(orientation);
    // glm::mat4 rot = glm::mat4_cast(orientation);

    // glm::mat4 trans = glm::mat4(1.0f);
    // trans = glm::translate(trans, -1.0f * eye_);

    // glm::mat4 transform = rot * trans;

    // look_ = glm::vec3(transform * glm::vec4(look_, 1.0f));
    // up_ = glm::vec3(transform * glm::vec4(up_, 1.0f));
    // tangent_ = glm::vec3(transform * glm::vec4(tangent_, 1.0f));

    

    last_x_ = current_x_;
	last_y_ = current_y_;
	current_x_ = mouse_x;
	current_y_ = window_height_ - mouse_y;
	float delta_x = current_x_ - last_x_;
	float delta_y = current_y_ - last_y_;
    yaw += delta_x;
    pitch += delta_y;
	if (sqrt(delta_x * delta_x + delta_y * delta_y) < 1e-15)
		return;
	// if (mouse_x > view_width_)
	// 	return ;
	glm::vec3 mouse_direction = glm::normalize(glm::vec3(delta_x, delta_y, 0.0f));
	glm::vec2 mouse_start = glm::vec2(last_x_, last_y_);
	glm::vec2 mouse_end = glm::vec2(current_x_, current_y_);
	glm::uvec4 viewport = glm::uvec4(0, 0, view_width_, view_height_);

	bool drag_camera = drag_state_ && current_button_ == GLFW_MOUSE_BUTTON_RIGHT;
	bool drag_bone = drag_state_ && current_button_ == GLFW_MOUSE_BUTTON_LEFT;

		// glm::vec3 axis = glm::normalize(
		// 		orientation_ *
		// 		glm::vec3(mouse_direction.y, -mouse_direction.x, 0.0f)
		// 		);
		
		glm::mat4 yaw_rot = (glm::rotate(delta_x * .002f, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::mat4 pitch_rot = (glm::rotate(delta_y * -.002f, glm::vec3(1.0f, 0.0f, 0.0f)));
    
		// camera_rot_ = pitch_rot * yaw_rot * camera_rot_;
        // float old_y = up_[1];

		orientation_ = glm::mat3(glm::mat4(orientation_) * pitch_rot * yaw_rot);
		
		tangent_ = glm::column(orientation_, 0);
		up_ = glm::column(orientation_, 1);
        
		look_ = glm::column(orientation_, 2);
        std::cout << "tan: " << tangent_[0] << " " << tangent_[1] << " " << tangent_[2] << " " << std::endl;
        std::cout << "up_: " << up_[0] << " " << up_[1] << " " << up_[2] << " " << std::endl;
        std::cout << "look_: " << look_[0] << " " << look_[1] << " " << look_[2] << " " << std::endl;
        // up_ = glm::cross(tangent_, look_);
        // up_[1] = old_y;
        
    // float rotation_speed = .02f;
    // float y = (float) mouse_y;
    // float x = (float) mouse_x;
    // glm::vec3 tangent = glm::normalize(glm::cross(look_, up_));
    // float old_y = tangent[1];
    // glm::vec3 mouse_vec(glm::normalize(x * tangent - y * up_));
    // glm::vec3 rotate_vec(glm::normalize(glm::cross(mouse_vec, look_)));
    // glm::mat4 R = glm::rotate(rotation_speed, rotate_vec);
    // tangent = glm::normalize(glm::vec3(R * glm::vec4(tangent, 0.0f)));
    // tangent[1] = old_y;
    // up_ = glm::normalize(glm::vec3(R * glm::vec4(up_, 0.0f)));
    // up_ = up_ - glm::dot(up_, tangent) * tangent;
    // look_ = glm::normalize(glm::cross(up_, tangent));

}

void GUI::mouseButtonCallback(int button, int action, int mods)
{
	// if (current_x_ <= view_width_) {
	// 	drag_state_ = (action == GLFW_PRESS);
	// 	current_button_ = button;
	// 	return ;
	// }
	// FIXME: Key Frame Selection
}

void GUI::mouseScrollCallback(double dx, double dy)
{
	// if (current_x_ < view_width_)
	// 	return;
	// FIXME: Mouse Scrolling
}

void GUI::updateMatrices()
{
	// Compute our view, and projection matrices.
	if (fps_mode_)
		center_ = eye_ + camera_distance_ * look_;
	else
		eye_ = center_ - camera_distance_ * look_;

	view_matrix_ = glm::lookAt(eye_, center_, up_);
	light_position_ = glm::vec4(eye_, 1.0f);

	aspect_ = static_cast<float>(view_width_) / view_height_;
	projection_matrix_ =
		glm::perspective((float)(kFov * (M_PI / 180.0f)), aspect_, kNear, kFar);
	model_matrix_ = glm::mat4(1.0f);
}

MatrixPointers GUI::getMatrixPointers() const
{
	MatrixPointers ret;
	ret.projection = &projection_matrix_;
	ret.model= &model_matrix_;
	ret.view = &view_matrix_;
	return ret;
}

// bool GUI::setCurrentBone(int i)
// {
// 	if (i < 0 || i >= mesh_->getNumberOfBones())
// 		return false;
// 	current_bone_ = i;
// 	return true;
// }

float GUI::getCurrentPlayTime() const
{
	return 0.0f;
}


bool GUI::captureWASDUPDOWN(int key, int action)
{
    glm::vec3 directup = glm::vec3(0.0f, 1.0f, 0.0f);
	if (key == GLFW_KEY_W) {
		if (fps_mode_)
			// eye_ += zoom_speed_ * look_;
            displacement_ += zoom_speed_ * look_;
		else
			camera_distance_ -= zoom_speed_;
		return true;
	} else if (key == GLFW_KEY_S) {
		if (fps_mode_)
			// eye_ -= zoom_speed_ * look_;
            displacement_ -= zoom_speed_ * look_;
		else
			camera_distance_ += zoom_speed_;
		return true;
	} else if (key == GLFW_KEY_A) {
		if (fps_mode_){
			// eye_ -= pan_speed_ * tangent_;
            glm::vec3 temp_eye = eye_;
            temp_eye -= pan_speed_ * tangent_;
            if(abs(temp_eye[0]) > 5.0f || abs(temp_eye[2]) > 5.0f){
                displacement_ -= pan_speed_ * tangent_;
                eye_[0] = fmod(displacement_[0] , 10.0f);
                eye_[2] = fmod(displacement_[2] , 10.0f);
            } else {
                eye_ -= pan_speed_ * tangent_;
            }
            std::cout << "Eye pos: " << eye_[0] << ", " << eye_[1] << ", " << eye_[2] << std::endl; 
        }
		else
			center_ -= pan_speed_ * tangent_;
		return true;
	} else if (key == GLFW_KEY_D) {
		if (fps_mode_)
			// eye_ += pan_speed_ * tangent_;
            displacement_ += pan_speed_ * tangent_;
		else
			center_ += pan_speed_ * tangent_;
		return true;
	} else if (key == GLFW_KEY_DOWN) {
		if (fps_mode_)
			eye_ -= pan_speed_ * directup;
		else
			center_ -= pan_speed_ * directup;
		return true;
	} else if (key == GLFW_KEY_SPACE) {
		if (fps_mode_)
			eye_ += pan_speed_ * directup;
		else
			center_ += pan_speed_ * directup;
		return true;
	} else if (key == GLFW_KEY_UP) {
		if (fps_mode_)
			eye_ += pan_speed_ * directup;
		else
			center_ += pan_speed_ * directup;
		return true;
	}
	return false;
}


// Delegrate to the actual GUI object.
void GUI::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->keyCallback(key, scancode, action, mods);
}

void GUI::MousePosCallback(GLFWwindow* window, double mouse_x, double mouse_y)
{
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->mousePosCallback(mouse_x, mouse_y);
}

void GUI::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->mouseButtonCallback(button, action, mods);
}

void GUI::MouseScrollCallback(GLFWwindow* window, double dx, double dy)
{
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->mouseScrollCallback(dx, dy);
}
