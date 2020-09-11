#include "transmitter.hpp"

#include <iostream>

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
	current_point_ = ray_tracer->InitializeOrCallPoint(transform_.position);
	display_pattern_ = false;
	Reset();
	Update();
}


void Transmitter::Update()
{
	//if (!ray_tracer_->store_points && current_point_ != nullptr) delete current_point_;
	current_point_ = ray_tracer_->InitializeOrCallPoint(transform_.position);

	if(current_pattern_ != nullptr) UpdateRadiationPattern();
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
	std::cout << "average path loss: " << average_path_loss << std::endl;
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

	if (display_pattern_) {
		/*for (auto& [theta, phi_value] : current_pattern_->pattern_) {
			if ((skipper++) % 20 == 0) continue;
			for (auto& [phi, gain] : phi_value) {
				//if ((skipper++) % 50 == 0) continue;

				glm::mat4 trans = glm::rotate(glm::mat4(1.0f), glm::radians(theta), glm::vec3(1.0f, 0.0f, 0.0f));
				trans = glm::rotate(trans, glm::radians(phi), glm::vec3(0.0f, 1.0f, 0.0f));
				glm::vec3 new_direction = glm::normalize(trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); // todo implement later
				new_direction = glm::rotateY(new_direction, tx_theta - glm::radians(90.0f));
				new_direction = glm::rotateX(new_direction, tx_phi);

				float gain_lin = pow(10, gain / 10);
				float max_gain_lin = pow(10, current_pattern_->max_gain_ / 10);
				float min_gain_lin = pow(10, current_pattern_->min_gain_ / 10);
				float normalized_gain = (gain_lin + min_gain_lin) * 10 / (max_gain_lin - min_gain_lin);
				/*std::cout << "max gain: " << max_gain_lin << std::endl;
				std::cout << "n gain: " << normalized_gain << std::endl;
				std::cout << "max gain: " << min_gain_lin << std::endl;

				Ray* ray = new Ray(current_point_->position, glm::vec3(new_direction.x, new_direction.y, new_direction.z));
				ray->InitializeRay(normalized_gain);
				ray->SetRayColor(glm::vec4(.2f, .3f, .4f, 1.0f));
				rays_.push_back(ray);

			}
		}*/

	}
	float min_linear = pow(10, current_pattern_->min_gain_ / 10);
	float max_linear = pow(10, current_pattern_->max_gain_ / 10);
	float step = 0.1f;
	int distance = 10;
	for (float angle = 0.0f; angle < 360.0f; angle = angle + step) {
		//std::cout << "i : " <<  std::endl;
		float start_angle = glm::radians(angle);
		float end_angle = glm::radians(angle+step);
		// Harizontal Line
		auto harizontal_direction_1 = glm::rotateY(front_direction_, start_angle);
		auto harizontal_direction_2 = glm::rotateY(front_direction_, end_angle);
		float h_value_1 = current_pattern_->pattern_[glm::degrees(start_angle)][90.0f] - current_pattern_->min_gain_;
		float h_value_2 = current_pattern_->pattern_[glm::degrees(end_angle)][90.0f] - current_pattern_->min_gain_;
		// Vertical Line
		auto vertical_direction_1 = glm::rotateZ(up_direction_, start_angle);
		auto vertical_direction_2 = glm::rotateZ(up_direction_, end_angle);
		
		float v_value_1 = current_pattern_->pattern_[90.0f][glm::degrees(start_angle)] - current_pattern_->min_gain_;
		float v_value_2 = current_pattern_->pattern_[90.0f][glm::degrees(end_angle)] - current_pattern_->min_gain_;

		/*h_value_1 = pow(10, h_value_1 / 10);
		h_value_2 = pow(10, h_value_2 / 10);
		h_value_1 = (h_value_1 + min_linear) / max_linear;
		h_value_2 = (h_value_2 + min_linear) / max_linear;

		v_value_1 = pow(10, v_value_1 / 10);
		v_value_2 = pow(10, v_value_2 / 10);
		v_value_1 = (v_value_1 + min_linear) * 100.0f  / max_linear;
		v_value_2 = (v_value_2 + min_linear) * 100.0f / max_linear;*/

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

	int pattern_theta_angle = std::round(glm::degrees(theta_to_near_tx - theta_tx));
	int pattern_phi_angle = std::round(glm::degrees(phi_to_near_tx - phi_tx)) + 90;
	if (pattern_theta_angle < 0.0f) pattern_theta_angle += 360.0f;
	if (pattern_phi_angle < 0.0f) pattern_phi_angle += 360.0f;

	if (pattern_phi_angle > 180.0f){

		std::cout << "---------------------------------" << std::endl;
		std::cout << "theta: " << pattern_theta_angle << "(deg)" << std::endl;
		std::cout << "phi: " << pattern_phi_angle << "(deg)" << std::endl;
		std::cout << "!! pattern out of range: return min gain !!" << std::endl;
		std::cout << "---------------------------------" << std::endl;

		return current_pattern_->min_gain_;
	}
	std::cout << "---------------------------------" << std::endl;
	std::cout << "theta: " << pattern_theta_angle << "(deg)" << std::endl;
	std::cout << "phi: " << pattern_phi_angle << "(deg)" << std::endl;

	std::map<float, float> phi_value = (*current_pattern_->pattern_.lower_bound(pattern_theta_angle)).second;
	float tx_gain = (*phi_value.lower_bound(pattern_phi_angle)).second;

	std::cout << "tx_gain: " << tx_gain << "[dB]" << std::endl;
	std::cout << "---------------------------------" << std::endl;

	//return tx_gain;
	 return 0.0f;
}

Receiver* Transmitter::GetReceiver(unsigned int index)
{
	if (index >= receivers_.size()) return nullptr;
	return receivers_[index];
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

void Transmitter::Rotate(const Direction rotation, float delta_time)
{
	float angular = delta_time * rotation_speed_;
	//std::cout << "angular : " << angular  << std::endl;
	switch(rotation) {
	case kLeft: {
		//front_direction_ = glm::rotateX(front_direction_, -angular);
		front_direction_ = glm::rotateY(front_direction_, angular);
		break;
	}
	case kRight: {
		front_direction_ = glm::rotateY(front_direction_, -angular);
		break;
	}
	case kUp: {
		up_direction_ = glm::rotateX(up_direction_, angular);
		break;
	}
	case kDown: {
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
