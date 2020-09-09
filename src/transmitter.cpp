#include "transmitter.hpp"

#include <iostream>

#include "ray.hpp"
#include "record.hpp"
#include "object.hpp"
#include "receiver.hpp"

#include "radiation_pattern.hpp"
#include "ray_tracer.hpp"


#include <math.h>

#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/matrix_transform.hpp>



Transmitter::Transmitter(Transform transform,
						float frequency, 
						RayTracer * ray_tracer):	transform_(transform), 
													frequency_(frequency),
													ray_tracer_(ray_tracer)
{
	current_pattern_ = nullptr;
	current_point_ = ray_tracer->InitializeOrCallPoint(transform_.position);

	Reset();
	Update();
}


void Transmitter::Update()
{
	current_point_ = ray_tracer_->InitializeOrCallPoint(transform_.position);
	for (auto* receiver : receivers_) {
		receiver->Update();
	}
}

void Transmitter::Reset()
{
	rotation_speed_ = 1.0f;
	move_speed_ = 10.0f;
	front_direction_ = glm::vec3(1.0f, 0.0f, 0.0);
	up_direction_ = glm::vec3(0.0f, 1.0f, 0.0);
}

void Transmitter::DrawObjects(Camera* camera)
{
	DrawRadiationPattern(camera);
	for (auto receiver : receivers_) {
		receiver->DrawObjects(camera);
	}
}

void Transmitter::AddReceiver(Receiver* receiver)
{
	receivers_.push_back(receiver);
}

void Transmitter::DrawRadiationPattern(Camera * camera)
{
	for (auto & ray : rays_) {
		ray->DrawObject(camera);
	}
}

void Transmitter::AssignRadiationPattern(RadiationPattern* pattern)
{
	current_pattern_ = pattern;
	rays_.clear();
	int skipper = 0;
	for (auto & [theta, phi_value ] : current_pattern_->pattern_) {
		//if ((skipper++) % 50)continue;
		for (auto& [phi, gain] : phi_value) {
			glm::mat4 trans = glm::rotate(glm::mat4(1.0f), glm::radians(phi), glm::vec3(1.0f, 0.0f, 0.0f));
			trans = glm::rotate(trans, glm::radians(theta), glm::vec3(0.0f, 1.0f, .0f));
			auto new_direction = glm::normalize(trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); // todo implement later
			Ray* ray = new Ray(transform_.position, glm::vec3(new_direction.x, new_direction.y, new_direction.z));
			
			float gain_lin = pow(10, gain / 10.f);
			float max_gain_lin = pow(10, pattern->max_gain_ / 10.0f);
			float min_gain_lin = pow(10, pattern->min_gain_ / 10.0f);
			float normalized_gain = gain_lin * 10.0f / (max_gain_lin - min_gain_lin);

			if (normalized_gain >= 1e-3) ray->InitializeRay(normalized_gain * 10.0f);
			//std::cout << " is " << j << std::endl;
			rays_.push_back(ray);
		}
	};
}

float Transmitter::GetFrequency()
{
	return frequency_;
}

Point* Transmitter::GetPoint()
{
	return current_point_;
}

float Transmitter::GetTransmitterGain(glm::vec3 near_tx_position)
{
	glm::vec3 tx_position = current_point_->position;
	glm::vec3 tx_to_point_direction = glm::normalize(near_tx_position - tx_position);
	const glm::vec3 front_direction = glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 up_direction = glm::vec3(0.0f, 1.0f, 0.0f);

	float theta_to_near_tx = glm::angle(front_direction, tx_to_point_direction );
	float phi_to_near_tx = glm::angle(up_direction, tx_to_point_direction);

	float theta_tx = glm::angle(front_direction, front_direction_);
	float phi_tx = glm::angle(up_direction, up_direction_);


	std::cout << "theta: " << glm::degrees(theta_to_near_tx) << ", " << glm::degrees(phi_to_near_tx) << "\n";

	float pattern_theta_angle = glm::degrees(theta_to_near_tx - theta_tx);
	float pattern_phi_angle = glm::degrees(phi_to_near_tx - phi_tx);
	std::map<float, float> phi_value = (*current_pattern_->pattern_.lower_bound(pattern_theta_angle)).second;
	float value = (*phi_value.lower_bound(pattern_phi_angle)).second;
	std::cout << "tx_gain: " << value << " [dB], linear: " << pow(10, value / 10) << std::endl;
	return value;
}

void Transmitter::Move(const Direction direction, float delta_time)
{
	float distance = delta_time * move_speed_;
	switch (direction) {
	case kForward:
		transform_.position += front_direction_ * distance;
		break;
	case kBackward:
		transform_.position -= front_direction_ * distance;
		break;
	case kRight:
		glm::vec3 left_direction = glm::normalize(glm::cross(front_direction_, up_direction_));
		transform_.position += left_direction * distance;
		break;
	case kLeft:
		glm::vec3 right_direction = glm::normalize(glm::cross(-front_direction_, up_direction_));
		transform_.position += right_direction * distance;
		break;
	case kUp:
		transform_.position += up_direction_ * distance;
		break;
	case kDown:
		transform_.position -= up_direction_ * distance;
		break;
	};
	Update();
}

void Transmitter::Rotate(const Rotation rotation, float delta_time)
{
	float angular = delta_time * rotation_speed_;
	switch(rotation) {
	case kYaw: {
		front_direction_ = glm::rotateX(front_direction_, angular);
		break;
	}
	case kPitch: {
		up_direction_ = glm::rotateY(up_direction_, angular);
		break;
	}
	}
}
