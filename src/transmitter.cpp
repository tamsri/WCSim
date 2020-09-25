#include "transmitter.hpp"

#include <iostream>

#include "triangle.hpp"
#include "object.hpp"
#include "ray.hpp"
#include "line.hpp"

#include "record.hpp"
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
	display_pattern_ = false;
	Reset();
	Update();
}


void Transmitter::Update()
{
	//if (current_point_ != nullptr) delete current_point_;
	current_point_ = new Point(transform_.position);

	//if(current_pattern_ != nullptr) UpdateRadiationPattern();

	if (receivers_.empty()) return;
	float average_path_loss = 0.0f;
	unsigned int receivers_number = 0;
	for (auto* receiver : receivers_) {
		receiver->Update();
		Result result = receiver->GetResult();
		if (result.is_valid) {
			average_path_loss += result.total_loss;
			++receivers_number;
		}
	}
	average_path_loss /= receivers_number;
	std::cout << "Average path loss: " << average_path_loss << std::endl;
}

void Transmitter::UpdateRadiationPattern()
{
	Clear();
	int skipper = 0;
	float tx_theta = glm::angle(front_direction_, glm::vec3(1.0f, 0.0f, 0.0f));
	float tx_phi = glm::angle(up_direction_, glm::vec3(0.0f, 1.0f, 0.0f));
	//std::cout << "tx_theta_angle : " << glm::degrees(tx_theta) << std::endl;
	//std::cout << "tx_phi_angle: " << glm::degrees(tx_phi) << std::endl;
	glm::vec3 position = current_point_->position;
	Line* front_ray = new Line(position, position + front_direction_*5.0f);
	front_ray->SetColor(glm::vec4(0.f, .6f, .5f, 1.0f));
	
	Line* up_ray = new Line(position, position + up_direction_ * 5.0f);
	up_ray->SetColor(glm::vec4(0.f, .6f, .5f, 1.0f));

	objects_.push_back(front_ray);
	objects_.push_back(up_ray);
	if (!display_pattern_) return;
	float min_linear = pow(10, current_pattern_->min_gain_ / 10);
	float max_linear = pow(10, current_pattern_->max_gain_ / 10);
	float min_max_linear = max_linear - min_linear;
	float min_dB =  current_pattern_->min_gain_;
	float max_dB = current_pattern_->max_gain_;
	float step = 2.0f;
	int distance = 10;
	const bool display_dB = false;
	for (float angle = 0.0f; angle < 360.0f; angle = angle + step) {
		//std::cout << "i : " <<  std::endl;
		const float start_angle = glm::radians(angle);
		const float end_angle = glm::radians(angle+step);
		constexpr float harizontal_angle = glm::radians(90.0f);
		constexpr float vertical_angle = glm::radians(0.0f);
		// Harizontal Line
		auto harizontal_direction_1 = glm::rotateY(front_direction_, start_angle);
		auto harizontal_direction_2 = glm::rotateY(front_direction_, end_angle);
		float h_value_1 = current_pattern_->GetGain(start_angle, harizontal_angle) ;
		float h_value_2 = current_pattern_->GetGain(end_angle, harizontal_angle) ;
		// Normalize the pattern
		if (!display_dB) {
			h_value_1 = pow(10, h_value_1 / 10) / (min_max_linear);
			h_value_2 = pow(10, h_value_2 / 10) / (min_max_linear);
		}
		else {
			h_value_1 = (h_value_1 + abs(min_dB)) / (max_dB - 2 * min_dB);
			h_value_2 = (h_value_2 + abs(min_dB)) / (max_dB - 2 * min_dB);
		}
		h_value_1 *= distance;
		h_value_2 *= distance;
		// Vertical Line
		auto vertical_direction_1 = glm::rotateZ(-up_direction_, start_angle);
		auto vertical_direction_2 = glm::rotateZ(-up_direction_, end_angle);
		float v_value_1;
		float v_value_2;
		if (angle >= 180.0f) {
			v_value_1 = current_pattern_->GetGain(glm::radians(180.0f), glm::radians(180.0f)-start_angle);
			v_value_2 = current_pattern_->GetGain(glm::radians(180.0f), glm::radians(180.0f)-end_angle);
		}
		else {
			v_value_1 = current_pattern_->GetGain(vertical_angle, start_angle);
			v_value_2 = current_pattern_->GetGain(vertical_angle, end_angle);
		}
		// Normalize the pattern
		if (!display_dB) {
			v_value_1 = pow(10, v_value_1 / 10) / (min_max_linear);
			v_value_2 = pow(10, v_value_2 / 10) / (min_max_linear);
		}
		else {
			v_value_1 = (v_value_1 +abs(min_dB)) / (max_dB - 2 * min_dB);
			v_value_2 = (v_value_2 +abs(min_dB)) /(max_dB - 2 * min_dB);
		}
		v_value_1 *= distance;
		v_value_2 *= distance;
		Line* harizontal_line = new Line(	position + harizontal_direction_1 * h_value_1, 
											position + harizontal_direction_2 * h_value_2);
		Line* vertical_line = new Line(		position + vertical_direction_1 * v_value_1, 
											position + vertical_direction_2 * v_value_2);

		harizontal_line->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		vertical_line->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		objects_.push_back(harizontal_line);
		objects_.push_back(vertical_line);
	}
	
}

void Transmitter::Reset()
{
	rotation_speed_ = .5f;
	move_speed_ = 10.0f;
	front_direction_ = glm::vec3(1.0f, 0.0f, 0.0);
	up_direction_ = glm::vec3(0.0f, 1.0f, 0.0);
}

void Transmitter::Clear()
{
	for (Object * object : objects_)
		delete object;
	objects_.clear();
}

void Transmitter::DrawObjects(Camera* camera)
{
	DrawRadiationPattern(camera);
	for (auto receiver : receivers_) {
		receiver->DrawObjects(camera);
	}
	if (current_pattern_ != nullptr)
		current_pattern_->DrawPattern(camera, transform_);
}

void Transmitter::AddReceiver(Receiver* receiver)
{
	receivers_.push_back(receiver);
}

void Transmitter::DrawRadiationPattern(Camera * camera)
{
	for (auto & ray : objects_) {
		ray->DrawObject(camera);
	}
}

void Transmitter::AssignRadiationPattern(RadiationPattern* pattern)
{
	current_pattern_ = pattern;
	UpdateRadiationPattern();
}

float Transmitter::GetFrequency()
{
	return frequency_;
}

float Transmitter::GetTransmitterGain(glm::vec3 near_tx_position)
{
	/*glm::vec3 tx_position = current_point_->position;
	glm::vec3 tx_to_point_direction = glm::normalize(near_tx_position - tx_position);
	glm::vec3 left_direction = glm::cross(front_direction_, up_direction_);
	std::cout << "left: " << left_direction.x << ", " << left_direction.y << ", " << left_direction.z << std::endl;
	float angle_to_front = glm::degrees(glm::angle(front_direction_, tx_to_point_direction ));
	float angle_to_up = glm::degrees(glm::angle(up_direction_, tx_to_point_direction));
	float angle_to_left = glm::degrees(glm::angle(left_direction, tx_to_point_direction));
	
	float phi_to_near_tx = angle_to_up;
	float theta_to_near_tx; 
	if (angle_to_left <= 90.0f) {
		theta_to_near_tx = angle_to_front;
	}
	else {
		theta_to_near_tx = 360.0f - angle_to_front;
	}

	std::cout << "theta: " << theta_to_near_tx  << ", phi:  " << phi_to_near_tx << "\n";
	float tx_gain = current_pattern_->GetGain(theta_to_near_tx, phi_to_near_tx);

	std::cout << "tx_gain: " << tx_gain << "[dB]" << std::endl;
	std::cout << "---------------------------------" << std::endl;*/

	// Calculate Theta (X, Z) plane angle
	glm::vec3 tx_position = current_point_->position;
	glm::vec2 tx_on_xz = glm::vec2(tx_position.x, tx_position.z);
	glm::vec2 near_tx_on_xz = glm::vec2(near_tx_position.x, near_tx_position.z);
	glm::vec2 to_near_tx_on_xz_direction = glm::normalize(near_tx_on_xz - tx_on_xz);
	glm::vec2 front_on_xz_direction = glm::vec2(front_direction_.x, front_direction_.z);
	float angle_on_front = glm::angle(front_on_xz_direction, to_near_tx_on_xz_direction);
	
	glm::vec2 tx_on_xy = glm::vec2(tx_position.x, tx_position.y);
	glm::vec2 near_tx_on_xy = glm::vec2(near_tx_position.x, near_tx_position.y);
	glm::vec2 to_near_tx_on_xy_direction = glm::normalize(near_tx_on_xy - tx_on_xy);
	glm::vec2 up_on_xy_direction = glm::vec2(up_direction_.x, up_direction_.y);
	float phi = glm::angle(up_on_xy_direction, to_near_tx_on_xy_direction);

	glm::vec3 left_direction = glm::cross(front_direction_, up_direction_);
	glm::vec3 tx_to_near_direction = glm::normalize(near_tx_position - tx_position);
	float angle_to_left = glm::degrees(glm::angle(left_direction, tx_to_near_direction));
	float theta;
	if (angle_to_left <= 90.0f) {
		theta = glm::radians(360.0f - glm::degrees(angle_on_front));
	}
	else {
		theta = angle_on_front;
	}
	std::cout << "theta: " << glm::degrees(theta) << ", phi: " << glm::degrees(phi); 

	float tx_gain = current_pattern_->GetGain(theta, phi);
	std::cout << " total gain: " << tx_gain << std::endl;
	return tx_gain;
}

Receiver* Transmitter::GetReceiver(unsigned int index)
{
	if (index >= receivers_.size()) return nullptr;
	return receivers_[index];
}

glm::vec3 Transmitter::GetPosition()
{
	return glm::vec3(transform_.position);
}

void Transmitter::Move(const Direction direction, float delta_time)
{
	float distance = delta_time * move_speed_;
	switch (direction) {
	case Direction::kForward:
		transform_.position += front_direction_ * distance;
		break;
	case Direction::kBackward:
		transform_.position -= front_direction_ * distance;
		break;
	case Direction::kRight:
		glm::vec3 left_direction = glm::normalize(glm::cross(front_direction_, up_direction_));
		transform_.position += left_direction * distance;
		break;
	case Direction::kLeft:
		glm::vec3 right_direction = glm::normalize(glm::cross(-front_direction_, up_direction_));
		transform_.position += right_direction * distance;
		break;
	case Direction::kUp:
		transform_.position += up_direction_ * distance;
		break;
	case Direction::kDown:
		transform_.position -= up_direction_ * distance;
		break;
	};
	Update();
}

void Transmitter::Rotate(const Direction rotation, float delta_time)
{
	float angular = delta_time * rotation_speed_;
	//std::cout << "angular : " << angular  << std::endl;
	switch(rotation) {
	case Direction::kLeft: {
		//front_direction_ = glm::rotateX(front_direction_, -angular);
		front_direction_ = glm::rotateY(front_direction_, angular);
		break;
	}
	case Direction::kRight: {
		front_direction_ = glm::rotateY(front_direction_, -angular);
		break;
	}
	case Direction::kUp: {
		up_direction_ = glm::rotateX(up_direction_, angular);
		break;
	}
	case Direction:: kDown: {
		up_direction_ = glm::rotateX(up_direction_, -angular);
		break;
	}
	}
	Update();
}

void Transmitter::ToggleDisplay()
{
	display_pattern_ = !display_pattern_;
	UpdateRadiationPattern();
}
