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
#include <glm/gtx/string_cast.hpp>

unsigned int Transmitter::global_id_ = 0;

Transmitter::Transmitter(Transform transform,
						float frequency,
						float transmit_power,
						RayTracer * ray_tracer):	id_(global_id_++),
													transform_(transform), 
													frequency_(frequency),
													transmit_power_(transmit_power),
													ray_tracer_(ray_tracer)
{
	current_pattern_ = nullptr;
	Reset();
}

Transmitter::~Transmitter()
{
	// TODO: implement de-constructor.
	receivers_.clear();
}

unsigned int Transmitter::GetID() const
{
	return id_;
}


void Transmitter::UpdateResult()
{

	if (receivers_.empty()) return;
	for (auto itr = receivers_.begin(); itr != receivers_.end(); ++itr) {
		itr->second->UpdateResult();
	}
}

void Transmitter::Reset()
{
	rotation_speed_ = .5f;
	move_speed_ = 10.0f;
	transform_.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
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
	for (auto & itr : receivers_) {
		auto * receiver = itr.second;
		receiver->DrawObjects(camera);
	}
	if (current_pattern_ != nullptr)
		current_pattern_->DrawPattern(camera, transform_);
}

void Transmitter::ConnectAReceiver(Receiver* receiver)
{
	if (receiver == nullptr) return;
	unsigned int id = receiver->GetID();
	if (receivers_.find(id) != receivers_.end()) return;
	receiver->ConnectATransmitter(this);
	receivers_.insert(std::make_pair(id, receiver));
}

void Transmitter::DisconnectAReceiver(unsigned int receiver_id)
{
	receivers_.erase(receiver_id);
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
}

void Transmitter::MoveTo(glm::vec3 position)
{
	transform_.position = position;
	UpdateResult();
}

void Transmitter::RotateTo(glm::vec3 rotation)
{
	transform_.rotation = rotation;
    UpdateResult();
}

Transform Transmitter::GetTransform() const
{
	return transform_;
}

float Transmitter::GetFrequency() const
{
	return frequency_;
}

float Transmitter::GetTransmitterGain(glm::vec3 near_tx_position)
{
	if (current_pattern_== nullptr) return 0.0f;

	glm::vec3 tx_position = transform_.position;
	float tx_theta = transform_.rotation.x;
	float tx_phi = transform_.rotation.y;

	glm::mat4 trans = glm::rotate(glm::mat4(1.0f), -tx_theta, glm::vec3(0.0f, 1.0f, 0.0f));
	trans = glm::rotate(trans, -tx_phi, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::vec3 front_direction = glm::vec3(trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	glm::vec3 up_direction = glm::vec3(trans * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	glm::vec2 tx_on_xz = glm::vec2(tx_position.x, tx_position.z);
	glm::vec2 near_tx_on_xz = glm::vec2(near_tx_position.x, near_tx_position.z);
	glm::vec2 to_near_tx_on_xz_direction = glm::normalize(near_tx_on_xz - tx_on_xz);
	glm::vec2 front_on_xz_direction = glm::vec2(front_direction.x, front_direction.z);
	float angle_on_front = glm::angle(front_on_xz_direction, to_near_tx_on_xz_direction);
	
	glm::vec2 tx_on_xy = glm::vec2(tx_position.x, tx_position.y);
	glm::vec2 near_tx_on_xy = glm::vec2(near_tx_position.x, near_tx_position.y);
	glm::vec2 to_near_tx_on_xy_direction = glm::normalize(near_tx_on_xy - tx_on_xy);
	glm::vec2 up_on_xy_direction = glm::vec2(up_direction.x, up_direction.y);
	float phi = glm::angle(up_on_xy_direction, to_near_tx_on_xy_direction);

	glm::vec3 left_direction = glm::cross(front_direction, up_direction);
	glm::vec3 tx_to_near_direction = glm::normalize(near_tx_position - tx_position);
	float angle_to_left = glm::degrees(glm::angle(left_direction, tx_to_near_direction));
	float theta;
	if (angle_to_left <= 90.0f) {
		theta = glm::radians(360.0f - glm::degrees(angle_on_front));
	}
	else {
		theta = angle_on_front;
	}

	// std::cout << "theta: " << glm::degrees(theta) << ", phi: " << glm::degrees(phi);
	float tx_gain = current_pattern_->GetGain(theta, phi);
	// std::cout << " total gain: " << tx_gain << std::endl;
	return tx_gain;
}

std::unordered_map<unsigned int, Receiver*>& Transmitter::GetReceivers()
{
	return receivers_;
}

std::string Transmitter::GetReceiversIDs()
{
    if (receivers_.empty()) return "0";

	std::string answer = std::to_string(receivers_.size()) + "&";
	for(auto itr = receivers_.begin(); itr != receivers_.end(); ++itr){
        answer += std::to_string(itr->first) + ",";
	}
	answer.pop_back();

	return answer;
}

glm::vec3 Transmitter::GetPosition()
{
	return glm::vec3(transform_.position);
}

void Transmitter::Move(const Direction direction, float delta_time)
{
	float distance = delta_time * move_speed_;
	glm::vec3 & rotation = transform_.rotation;

	glm::mat4 trans = glm::rotate(glm::mat4(1.0f), -transform_.rotation.x, glm::vec3(0.0f, 1.0f, 0.0f));
	trans = glm::rotate(trans, -transform_.rotation.y, glm::vec3(0.0f, 0.0f, 1.0f));
	
	glm::vec3 front_direction;  
	glm::vec3 up_direction;
	glm::vec3 right_direction;

	constexpr glm::vec3 x_axis = glm::vec3(1.0f, 0.0f, 0.0f);
	constexpr glm::vec3 y_axis = glm::vec3(0.0f, 1.0f, 0.0f);

	switch (direction) {
	case (Direction::kForward):{
		front_direction = glm::vec3(trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		transform_.position += front_direction * distance;
	}	break;
	case (Direction::kBackward):{
		front_direction = glm::vec3(trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		transform_.position -= front_direction * distance;
	}	break;
	case (Direction::kRight):{
		front_direction = glm::vec3(trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		up_direction = glm::vec3(trans * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		right_direction = glm::cross(front_direction, up_direction);
		transform_.position += right_direction * distance;
	}	break;
	case (Direction::kLeft): {
		front_direction = glm::vec3(trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		up_direction = glm::vec3(trans * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		right_direction = glm::cross(front_direction, up_direction);
		transform_.position -= right_direction * distance;
	}	break;
	case (Direction::kUp): {
		up_direction = glm::vec3(trans * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		transform_.position += up_direction * distance;
	}	break;
	case (Direction::kDown): {
		up_direction = glm::vec3(trans * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		transform_.position -= up_direction * distance;
	}	break;
	}
	UpdateResult();
}

void Transmitter::Rotate(const Direction rotation, float delta_time)
{
	float angular = delta_time * rotation_speed_;
	switch(rotation) {
	case (Direction::kLeft): {
		transform_.rotation.x += angular;
	}break;
	case (Direction::kRight): {
		transform_.rotation.x -= angular;
	}break;
	case (Direction::kUp): {
		transform_.rotation.y -= angular;
	}break;
	case (Direction:: kDown): {
		transform_.rotation.y += angular;
	}break;
	}
	UpdateResult();
}

float Transmitter::GetTransmitPower() const {
    return transmit_power_;
}


