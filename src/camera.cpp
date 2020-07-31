#include "camera.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "window.hpp"

Camera::Camera(Window* window) :window_(window) {
	Reset();
}

void Camera::Move(const CameraDirection camera_direction, float delta_time)
{
	float distance = delta_time * camera_move_speed_;
	switch (camera_direction) {
	case kForward:
		position_ += front_direction_ * distance;
		break;
	case kBackward:
		position_ -= front_direction_ * distance;
		break;
	case kRight:
		glm::vec3 left_direction = glm::normalize(glm::cross(front_direction_, up_direction_));
		position_ += left_direction * distance;
		break;

	case kLeft:
		glm::vec3 right_direction = glm::normalize(glm::cross(-front_direction_, up_direction_));
		position_ += right_direction * distance;
		break;
	case kUp:
		position_ += up_direction_ * distance;
		break;
	case kDown:
		position_ -= up_direction_ * distance;
		break;
	};

	UpdateView();
}

void Camera::Rotate(const CameraRotation camera_rotation, float delta_time)
{
	float angle = camera_rotate_speed_ * delta_time;
	switch (camera_rotation) {
	case kYaw:
		yaw_ += angle;
		break;
	case kRoll:
		/// TODO: to be implemented (maybe)
		break;
	case kPitch:
		pitch_ += angle;
		break;
	}
	UpdateRotation();
	UpdateView();
}

void Camera::Reset()
{
	fov_ = 45.0f;
	camera_move_speed_ = 10.0f;
	camera_rotate_speed_ = 100.0f;
	yaw_ = -90.0f;
	pitch_ = -10.0f;
	position_ = glm::vec3(0.0f, 2.0f, 5.0f);
	up_direction_ = glm::vec3(0.0f, 1.0f, 0.0f);
	UpdateRotation();
	UpdateView();
	UpdateProjection();
}

void Camera::UpdateProjection()
{
	projection_ = glm::perspective(glm::radians(fov_), (float)window_->GetWindowHeight() / (float)window_->GetWindowHeight(), 0.1f, 1000.0f);
}

void Camera::UpdateRotation()
{
	front_direction_.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
	front_direction_.y = sin(glm::radians(pitch_));
	front_direction_.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
	front_direction_ = glm::normalize(front_direction_);
}

void Camera::UpdateView()
{
	view_ = glm::lookAt(position_, position_ + front_direction_, up_direction_);
}

glm::mat4 Camera::GetView() const
{
	return view_;
}