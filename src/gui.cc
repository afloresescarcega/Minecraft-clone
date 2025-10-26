#include "gui.h"
#include "config.h"
#include <jpegio.h>
#include <iostream>
#include <algorithm>
#include <debuggl.h>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <math.h> // fmod



GUI::GUI(GLFWwindow* window, PerlinNoise *pn_, int view_width, int view_height, int preview_height)
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
	aspect_ = static_cast<float>(view_width_) / static_cast<float>(view_height_);
	projection_matrix_ = glm::perspective((float)(kFov * (M_PI / 180.0f)), aspect_, kNear, kFar);
    pn = pn_;
}

GUI::~GUI() = default;

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

	if (mods == 0 && captureWASDUPDOWN(key, action))
		return ;
	if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
	} else if (key == GLFW_KEY_C && action != GLFW_RELEASE) {
		fps_mode_ = !fps_mode_;
	} else if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_RELEASE) {
	} else if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_RELEASE) {
	} else if (key == GLFW_KEY_T && action != GLFW_RELEASE) {
	}

}

void GUI::mousePosCallback(double mouse_x, double mouse_y)
{
    last_x_ = current_x_;
	last_y_ = current_y_;
	current_x_ = mouse_x;
	current_y_ = window_height_ - mouse_y;
	float delta_x = current_x_ - last_x_;
	float delta_y = current_y_ - last_y_;
    yaw -= delta_x;
    pitch = std::max(-89.0f, std::min(89.0f, pitch + delta_y));
	if (sqrt(delta_x * delta_x + delta_y * delta_y) < 1e-15)
		return;
	glm::vec3 mouse_direction = glm::normalize(glm::vec3(delta_x, delta_y, 0.0f));
	glm::vec2 mouse_start = glm::vec2(last_x_, last_y_);
	glm::vec2 mouse_end = glm::vec2(current_x_, current_y_);
	glm::uvec4 viewport = glm::uvec4(0, 0, view_width_, view_height_);

    
    pitch = std::max(-89.0f, std::min(89.0f, pitch - delta_y * 0.002f));
    yaw += delta_x * 0.002f;
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    look_ = glm::vec3(rotation * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    up_ = glm::vec3(0.0f, 1.0f, 0.0f);
    tangent_ = glm::normalize(glm::cross(up_, look_));
}

void GUI::mouseButtonCallback(int button, int action, int mods)
{
}

void GUI::mouseScrollCallback(double dx, double dy)
{
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

float GUI::getCurrentPlayTime() const
{
	return 0.0f;
}


void GUI::wrapEyePosition() {
	//This creates a "torus-like" world where moving off one edge teleports you to the opposite edge,
	//keeping the player always within a 5-unit cube centered at (47.5, y, 47.5).
	// For within block movement at center of world
	if (eye_[0] < 45.0f) {
		eye_[0] = (eye_[0] - 45.0f) + 50.0f;
	} else if (eye_[0] > 50.0f) {
		eye_[0] = (eye_[0] - 50.0f) + 45.0f;
	} else if (eye_[2] < 45.0f) {
		eye_[2] = (eye_[2] - 45.0f) + 50.0f;
	} else if (eye_[2] > 50.0f) {
		eye_[2] = (eye_[2] - 50.0f) + 45.0f;
	}
}

bool GUI::captureWASDUPDOWN(int key, int action)
{
    float d_x = displacement_[0];
    float d_y = displacement_[1];
    float d_z = displacement_[2];
    float x = kTileLen* floor(eye_[0]/kTileLen);
    float eye_y  = kTileLen* floor(eye_[1]/kTileLen);
    float feet_y = kTileLen* floor((eye_[1] - kTileLen)/kTileLen);
    float z = kTileLen* floor(eye_[2]/kTileLen);
    moving_forward = false;
    glm::vec3 directup = glm::vec3(0.0f, 1.0f, 0.0f);
	if (key == GLFW_KEY_W && action != GLFW_RELEASE) { // Forward
        moving_forward = true;

		glm::vec3 pred_eye = eye_;
		glm::vec3 pred_displacement = displacement_;
		if (fps_mode_) {
			pred_displacement += zoom_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * look_;
			pred_eye          += zoom_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * look_;
		} else {
			pred_displacement += zoom_speed_ * look_;
			pred_eye += zoom_speed_ * look_;
		}

		d_x = pred_displacement[0];
		d_y = pred_displacement[1];
		d_z = pred_displacement[2];
		x = kTileLen* floor(pred_eye[0]/kTileLen);
		eye_y  = kTileLen* floor(pred_eye[1]/kTileLen);
		feet_y = kTileLen* floor((pred_eye[1] - kTileLen)/kTileLen);
		z = kTileLen* floor(pred_eye[2]/kTileLen);


		// negative heights mean air block		
        double top_height    = pn->octaveNoise(1/30.0f * ((double) x  + kTileLen* floor(d_x/kTileLen)) + .01, 1/30.0f * ((double) eye_y  + kTileLen* floor(d_y/kTileLen))+ .01, 1/30.0f * ((double) z +kTileLen * floor(d_z/kTileLen))+ .01, 3);
        double bottom_height = pn->octaveNoise(1/30.0f * ((double) x  + kTileLen* floor(d_x/kTileLen)) + .01, 1/30.0f * ((double) feet_y + kTileLen* floor(d_y/kTileLen))+ .01, 1/30.0f * ((double) z +kTileLen * floor(d_z/kTileLen))+ .01, 3);

        if (feet_y < 55.0 && (top_height > 0.0 || bottom_height > 0.0)) {
        } else {
			if (fps_mode_){
				displacement_ += zoom_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * look_;
				eye_          += zoom_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * look_;

				wrapEyePosition();

			}

			else {
				displacement_ += zoom_speed_ * look_;
				eye_ += zoom_speed_ * look_;

				std:: cout << eye_.y << std::endl;
				wrapEyePosition();
				std:: cout << eye_.y << std::endl;
			}
        }
	} else if (key == GLFW_KEY_S) { // Backward
		moving_forward = true;

		glm::vec3 pred_eye = eye_;
		glm::vec3 pred_displacement = displacement_;
		if (fps_mode_) {
			pred_displacement -= zoom_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * look_;
			pred_eye          -= zoom_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * look_;
		} else {
			pred_displacement -= zoom_speed_ * look_;
			pred_eye -= zoom_speed_ * look_;
		}

		d_x = pred_displacement[0];
		d_y = pred_displacement[1];
		d_z = pred_displacement[2];
		x = kTileLen* floor(pred_eye[0]/kTileLen);
		eye_y  = kTileLen* floor(pred_eye[1]/kTileLen);
		feet_y = kTileLen* floor((pred_eye[1] - kTileLen)/kTileLen);
		z = kTileLen* floor(pred_eye[2]/kTileLen);

        double top_height    = pn->octaveNoise(1/30.0f * ((double) x  + kTileLen* floor(d_x/kTileLen)) + .01, 1/30.0f * ((double) eye_y  + kTileLen* floor(d_y/kTileLen))+ .01, 1/30.0f * ((double) z +kTileLen * floor(d_z/kTileLen))+ .01, 3);
        double bottom_height = pn->octaveNoise(1/30.0f * ((double) x  + kTileLen* floor(d_x/kTileLen)) + .01, 1/30.0f * ((double) feet_y + kTileLen* floor(d_y/kTileLen))+ .01, 1/30.0f * ((double) z +kTileLen * floor(d_z/kTileLen))+ .01, 3);
        if (feet_y < 55.0 && (top_height > 0.0 || bottom_height > 0.0)) {
        } else {
			if (fps_mode_){
				displacement_ -= zoom_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * look_;
				eye_          -= zoom_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * look_;

				wrapEyePosition();
			}

			else {
				displacement_ -= zoom_speed_ * look_;
				eye_ -= zoom_speed_ * look_;

				wrapEyePosition();
			}
        }
	} else if (key == GLFW_KEY_A) { // Strafe left
		moving_forward = true;

		glm::vec3 pred_eye = eye_;
		glm::vec3 pred_displacement = displacement_;
		if (fps_mode_) {
			pred_displacement -= pan_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * tangent_;
			pred_eye          -= pan_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * tangent_;
		} else {
			pred_displacement -= pan_speed_ * tangent_;
			pred_eye -= pan_speed_ * tangent_;
		}

		d_x = pred_displacement[0];
		d_y = pred_displacement[1];
		d_z = pred_displacement[2];
		x = kTileLen* floor(pred_eye[0]/kTileLen);
		eye_y  = kTileLen* floor(pred_eye[1]/kTileLen);
		feet_y = kTileLen* floor((pred_eye[1] - kTileLen)/kTileLen);
		z = kTileLen* floor(pred_eye[2]/kTileLen);

		// negative heights mean air block		
        double top_height    = pn->octaveNoise(1/30.0f * ((double) x  + kTileLen* floor(d_x/kTileLen)) + .01, 1/30.0f * ((double) eye_y  + kTileLen* floor(d_y/kTileLen))+ .01, 1/30.0f * ((double) z +kTileLen * floor(d_z/kTileLen))+ .01, 3);
        double bottom_height = pn->octaveNoise(1/30.0f * ((double) x  + kTileLen* floor(d_x/kTileLen)) + .01, 1/30.0f * ((double) feet_y + kTileLen* floor(d_y/kTileLen))+ .01, 1/30.0f * ((double) z +kTileLen * floor(d_z/kTileLen))+ .01, 3);
        if (feet_y < 55.0 && (top_height > 0.0 || bottom_height > 0.0)) {
        } else {
			if (fps_mode_){
				displacement_ -= pan_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * tangent_;
				eye_          -= pan_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * tangent_;

				wrapEyePosition();
			}

			else {
				displacement_ -= pan_speed_ * tangent_;
				eye_ -= pan_speed_ * tangent_;
				wrapEyePosition();
			}
        }
	} else if (key == GLFW_KEY_D) { // Strafe right
		moving_forward = true;

		glm::vec3 pred_eye = eye_;
		glm::vec3 pred_displacement = displacement_;
		if (fps_mode_) {
			pred_displacement += pan_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * tangent_;
			pred_eye          += pan_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * tangent_;
		} else {
			pred_displacement += pan_speed_ * tangent_;
			pred_eye += pan_speed_ * tangent_;
		}

		d_x = pred_displacement[0];
		d_y = pred_displacement[1];
		d_z = pred_displacement[2];
		x = kTileLen* floor(pred_eye[0]/kTileLen);
		eye_y  = kTileLen* floor(pred_eye[1]/kTileLen);
		feet_y = kTileLen* floor((pred_eye[1] - kTileLen)/kTileLen);
		z = kTileLen* floor(pred_eye[2]/kTileLen);

		// negative heights mean air block		
        double top_height    = pn->octaveNoise(1/30.0f * ((double) x  + kTileLen* floor(d_x/kTileLen)) + .01, 1/30.0f * ((double) eye_y  + kTileLen* floor(d_y/kTileLen))+ .01, 1/30.0f * ((double) z +kTileLen * floor(d_z/kTileLen))+ .01, 3);
        double bottom_height = pn->octaveNoise(1/30.0f * ((double) x  + kTileLen* floor(d_x/kTileLen)) + .01, 1/30.0f * ((double) feet_y + kTileLen* floor(d_y/kTileLen))+ .01, 1/30.0f * ((double) z +kTileLen * floor(d_z/kTileLen))+ .01, 3);
        if (feet_y < 55.0 && (top_height > 0.0 || bottom_height > 0.0)) {
        } else {
			if (fps_mode_){
				displacement_ += pan_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * tangent_;
				eye_          += pan_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * tangent_;

				wrapEyePosition();
			}

			else {
				displacement_ += pan_speed_ * tangent_;
				eye_ += pan_speed_ * tangent_;
				wrapEyePosition();
			}
        }
	}
    
    if (key == GLFW_KEY_DOWN) { // Jump
		if (fps_mode_){
            feet_above_ground = true;
            y_velocity = 6.5f;
        }
		else
			center_ -= pan_speed_ * directup;
		return true;
	} if (key == GLFW_KEY_SPACE && !feet_above_ground) { // Jump
		if (fps_mode_){
            just_jumped = true;
            feet_above_ground = true;
        }
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
