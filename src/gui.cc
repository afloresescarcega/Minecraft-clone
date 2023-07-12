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
	float aspect_ = static_cast<float>(view_width_) / view_height_;
	projection_matrix_ = glm::perspective((float)(kFov * (M_PI / 180.0f)), aspect_, kNear, kFar);
    pn = pn_;
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
    glm::mat3 old_orientation = orientation_;
    float old_y = tangent_[1];
    orientation_ = glm::mat3(glm::mat4(orientation_) * pitch_rot * yaw_rot);
    // glm::vec3 new_tangent = glm::cross(glm::vec3(glm::column(old_orientation, 2)), glm::vec3(0.0f, 1.0f, 0.0f));
    tangent_ = glm::column(orientation_, 0);
    tangent_[1] = old_y;
    up_ = glm::column(orientation_, 1);
    up_ = up_ - glm::dot(up_,tangent_) * tangent_;

    look_ = glm::normalize(glm::cross(up_,tangent_));
    // std::cout << "tan: " << tangent_[0] << " " << tangent_[1] << " " << tangent_[2] << " " << std::endl;
    // std::cout << "up_: " << up_[0] << " " << up_[1] << " " << up_[2] << " " << std::endl;
    // std::cout << "look_: " << look_[0] << " " << look_[1] << " " << look_[2] << " " << std::endl;
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
        // glm::dvec3 temp_displacement_ = displacement_ +  (double) zoom_speed_ * glm::dvec3(1.0f, 0.0f, 1.0f) * glm::dvec3(look_.x, look_.y, look_.z);

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

		//std:: cout << eye_y << std::endl;
		//std:: cout << feet_y << std::endl;

		// negative heights mean air block		
        double top_height    = pn->octaveNoise(1/30.0f * ((double) x  + kTileLen* floor(d_x/kTileLen)) + .01, 1/30.0f * ((double) eye_y  + kTileLen* floor(d_y/kTileLen))+ .01, 1/30.0f * ((double) z +kTileLen * floor(d_z/kTileLen))+ .01, 3);
        double bottom_height = pn->octaveNoise(1/30.0f * ((double) x  + kTileLen* floor(d_x/kTileLen)) + .01, 1/30.0f * ((double) feet_y + kTileLen* floor(d_y/kTileLen))+ .01, 1/30.0f * ((double) z +kTileLen * floor(d_z/kTileLen))+ .01, 3);

        //std:: cout << top_height << std::endl;
        //std:: cout << bottom_height << std::endl;
        //std:: cout << "----" << std::endl;

        if (feet_y < 55.0 && (top_height > 0.0 || bottom_height > 0.0)) {
            //std::cout << "eye_.x " << eye_.x << " eye_.y " << eye_.y << " eye_.z " << eye_.z << std::endl;
            //std::cout << "Intersected: y " << eye_y << " top_height: " << top_height << " bottom_height: " << bottom_height << std::endl;
            //return true;
        } else {
            //std::cout << "not Intersected: y " << feet_y << " top_height: " << top_height << " bottom_height: " << bottom_height << std::endl;
			//std::cout << "moving" << std::endl;
			if (fps_mode_){
				displacement_ += zoom_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * look_;
				eye_          += zoom_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * look_;

				// For within block movement at center of world
				if(eye_[0] < 45.0f){
					eye_[0] = (eye_[0] - 45.0f) + 50.0f;
				} else if( eye_[0] > 50.0f){
					eye_[0] = (eye_[0] - 50.0f) + 45.0f;
				} else if(eye_[2] < 45.0f){
					eye_[2] = (eye_[2] - 45.0f) + 50.0f;
				} else if( eye_[2] > 50.0f){
					eye_[2] = (eye_[2] - 50.0f) + 45.0f;
				}

				// std::cout << "Eye_x: " << eye_[0] << " eye_y: " << eye_[1] << " eye_z: " << eye_[2] << std::endl;
			}
				// eye_ += zoom_speed_ * look_;
				
			else {
				displacement_ += zoom_speed_ * look_;
				eye_ += zoom_speed_ * look_;

				std:: cout << eye_.y << std::endl;
				if(eye_[0] < 45.0f){
					eye_[0] = (eye_[0] - 45.0f) + 50.0f;
				} else if( eye_[0] > 50.0f){
					eye_[0] = (eye_[0] - 50.0f) + 45.0f;
				} else if(eye_[2] < 45.0f){
					eye_[2] = (eye_[2] - 45.0f) + 50.0f;
				} else if( eye_[2] > 50.0f){
					eye_[2] = (eye_[2] - 50.0f) + 45.0f;
				}
				std:: cout << eye_.y << std::endl;
				// std::cout << "eye_x: " << eye_[0] << " eye_y: " << eye_[1] << " eye_z: " << eye_[2] << std::endl;
			}
        }
	} else if (key == GLFW_KEY_S) { // Backward
		moving_forward = true;
        // glm::dvec3 temp_displacement_ = displacement_ +  (double) zoom_speed_ * glm::dvec3(1.0f, 0.0f, 1.0f) * glm::dvec3(look_.x, look_.y, look_.z);

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
            //std::cout << "eye_.x " << eye_.x << " eye_.y " << eye_.y << " eye_.z " << eye_.z << std::endl;
            //std::cout << "Intersected: y " << eye_y << " top_height: " << top_height << " bottom_height: " << bottom_height << std::endl;
            //return true;
        } else {
            //std::cout << "not Intersected: y " << feet_y << " top_height: " << top_height << " bottom_height: " << bottom_height << std::endl;
			//std::cout << "moving" << std::endl;
			if (fps_mode_){
				displacement_ -= zoom_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * look_;
				eye_          -= zoom_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * look_;

				// For within block movement at center of world
				if(eye_[0] < 45.0f){
					eye_[0] = (eye_[0] - 45.0f) + 50.0f;
				} else if( eye_[0] > 50.0f){
					eye_[0] = (eye_[0] - 50.0f) + 45.0f;
				} else if(eye_[2] < 45.0f){
					eye_[2] = (eye_[2] - 45.0f) + 50.0f;
				} else if( eye_[2] > 50.0f){
					eye_[2] = (eye_[2] - 50.0f) + 45.0f;
				}
				// std::cout << "Eye_x: " << eye_[0] << " eye_y: " << eye_[1] << " eye_z: " << eye_[2] << std::endl;
			}
				// eye_ += zoom_speed_ * look_;
				
			else {
				displacement_ -= zoom_speed_ * look_;
				eye_ -= zoom_speed_ * look_;

				if(eye_[0] < 45.0f){
					eye_[0] = (eye_[0] - 45.0f) + 50.0f;
				} else if( eye_[0] > 50.0f){
					eye_[0] = (eye_[0] - 50.0f) + 45.0f;
				} else if(eye_[2] < 45.0f){
					eye_[2] = (eye_[2] - 45.0f) + 50.0f;
				} else if( eye_[2] > 50.0f){
					eye_[2] = (eye_[2] - 50.0f) + 45.0f;
				}
				// std::cout << "eye_x: " << eye_[0] << " eye_y: " << eye_[1] << " eye_z: " << eye_[2] << std::endl;
			}
        }
	} else if (key == GLFW_KEY_A) { // Strafe left
		moving_forward = true;
        // glm::dvec3 temp_displacement_ = displacement_ +  (double) zoom_speed_ * glm::dvec3(1.0f, 0.0f, 1.0f) * glm::dvec3(look_.x, look_.y, look_.z);

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
            //std::cout << "eye_.x " << eye_.x << " eye_.y " << eye_.y << " eye_.z " << eye_.z << std::endl;
            //std::cout << "Intersected: y " << eye_y << " top_height: " << top_height << " bottom_height: " << bottom_height << std::endl;
            //return true;
        } else {
            //std::cout << "not Intersected: y " << feet_y << " top_height: " << top_height << " bottom_height: " << bottom_height << std::endl;
			//std::cout << "moving" << std::endl;
			if (fps_mode_){
				displacement_ -= pan_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * tangent_;
				eye_          -= pan_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * tangent_;

				// For within block movement at center of world
				if(eye_[0] < 45.0f){
					eye_[0] = (eye_[0] - 45.0f) + 50.0f;
				} else if( eye_[0] > 50.0f){
					eye_[0] = (eye_[0] - 50.0f) + 45.0f;
				} else if(eye_[2] < 45.0f){
					eye_[2] = (eye_[2] - 45.0f) + 50.0f;
				} else if( eye_[2] > 50.0f){
					eye_[2] = (eye_[2] - 50.0f) + 45.0f;
				}
				// std::cout << "Eye_x: " << eye_[0] << " eye_y: " << eye_[1] << " eye_z: " << eye_[2] << std::endl;
			}
				// eye_ += zoom_speed_ * look_;
				
			else {
				displacement_ -= pan_speed_ * tangent_;
				eye_ -= pan_speed_ * tangent_;
				if(eye_[0] < 45.0f){
					eye_[0] = (eye_[0] - 45.0f) + 50.0f;
				} else if( eye_[0] > 50.0f){
					eye_[0] = (eye_[0] - 50.0f) + 45.0f;
				} else if(eye_[2] < 45.0f){
					eye_[2] = (eye_[2] - 45.0f) + 50.0f;
				} else if( eye_[2] > 50.0f){
					eye_[2] = (eye_[2] - 50.0f) + 45.0f;
				}
				// std::cout << "eye_x: " << eye_[0] << " eye_y: " << eye_[1] << " eye_z: " << eye_[2] << std::endl;
			}
        }
	} else if (key == GLFW_KEY_D) { // Strafe right
		moving_forward = true;
        // glm::dvec3 temp_displacement_ = displacement_ +  (double) zoom_speed_ * glm::dvec3(1.0f, 0.0f, 1.0f) * glm::dvec3(look_.x, look_.y, look_.z);

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
            //std::cout << "eye_.x " << eye_.x << " eye_.y " << eye_.y << " eye_.z " << eye_.z << std::endl;
            //std::cout << "Intersected: y " << eye_y << " top_height: " << top_height << " bottom_height: " << bottom_height << std::endl;
            //return true;
        } else {
            //std::cout << "not Intersected: y " << feet_y << " top_height: " << top_height << " bottom_height: " << bottom_height << std::endl;
			//std::cout << "moving" << std::endl;
			if (fps_mode_){
				displacement_ += pan_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * tangent_;
				eye_          += pan_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * tangent_;

				// For within block movement at center of world
				if(eye_[0] < 45.0f){
					eye_[0] = (eye_[0] - 45.0f) + 50.0f;
				} else if( eye_[0] > 50.0f){
					eye_[0] = (eye_[0] - 50.0f) + 45.0f;
				} else if(eye_[2] < 45.0f){
					eye_[2] = (eye_[2] - 45.0f) + 50.0f;
				} else if( eye_[2] > 50.0f){
					eye_[2] = (eye_[2] - 50.0f) + 45.0f;
				}
				// std::cout << "Eye_x: " << eye_[0] << " eye_y: " << eye_[1] << " eye_z: " << eye_[2] << std::endl;
			}
				// eye_ += zoom_speed_ * look_;
				
			else {
				displacement_ += pan_speed_ * tangent_;
				eye_ += pan_speed_ * tangent_;
				if(eye_[0] < 45.0f){
					eye_[0] = (eye_[0] - 45.0f) + 50.0f;
				} else if( eye_[0] > 50.0f){
					eye_[0] = (eye_[0] - 50.0f) + 45.0f;
				} else if(eye_[2] < 45.0f){
					eye_[2] = (eye_[2] - 45.0f) + 50.0f;
				} else if( eye_[2] > 50.0f){
					eye_[2] = (eye_[2] - 50.0f) + 45.0f;
				}
				// std::cout << "eye_x: " << eye_[0] << " eye_y: " << eye_[1] << " eye_z: " << eye_[2] << std::endl;
			}
        }
	}
    
    if (key == GLFW_KEY_DOWN) { // Jump
		if (fps_mode_){
			// eye_ -= pan_speed_ * directup;
            feet_above_ground = true;
            y_velocity = 6.5f;
        }
		else
			center_ -= pan_speed_ * directup;
		return true;
	} if (key == GLFW_KEY_SPACE && !feet_above_ground) { // Jump
		if (fps_mode_){
			// eye_ += 15.0f * directup;
            just_jumped = true;
            feet_above_ground = true;
            // y_velocity = 6.5f;
            // feet_above_ground = true;
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
