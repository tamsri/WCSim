#include "receiver.hpp"

#include <iostream>

#include "object.hpp"
#include "ray_tracer.hpp"
#include "transmitter.hpp"
#include "recorder.hpp"

#include <glm/gtx/string_cast.hpp>

unsigned int Receiver::global_id_ = 0;
Receiver::Receiver(Transform transform, RayTracer* ray_tracer):	id_(global_id_++),
														transform_(transform),
														ray_tracer_(ray_tracer),
														transmitter_(nullptr),
														recorder_(nullptr){
	Reset();
}

Receiver::Receiver(Transform transform, RayTracer * ray_tracer, Transmitter * transmitter): 
	id_(global_id_++),
	transform_(transform),	
	ray_tracer_(ray_tracer),
	transmitter_(transmitter),
	recorder_(nullptr)
{
	Reset();
}

unsigned int Receiver::GetID() const
{
	return id_;
}

void Receiver::ConnectATransmitter(Transmitter* transmitter)
{
	transmitter_ = transmitter;
}

void Receiver::DisconnectATransmitter()
{
	transmitter_ = nullptr;
}

Transmitter* Receiver::GetTransmitter() const
{
	return transmitter_;
}

Transform Receiver::GetTransform() const
{
	return transform_;
}

void Receiver::AddRecorder(Recorder* recorder)
{
	recorder_ = recorder;
}

Result Receiver::GetResult() const
{
	return result_;
}

glm::vec3 Receiver::GetPosition() const
{
	return glm::vec3(transform_.position);
}

void Receiver::UpdateResult()
{
	if (transmitter_ == nullptr) return;
	const glm::vec3 receiver_position = transform_.position;
	const glm::vec3 transmitter_positon = transmitter_->GetPosition();
	float transmitter_frequency = transmitter_->GetFrequency();


	records_.clear();
	ray_tracer_->Trace(transmitter_positon, receiver_position, records_);
	
	ray_tracer_->CalculatePathLoss(transmitter_positon, receiver_position, transmitter_frequency, records_, result_, recorder_ );

	/*if (ray_tracer_->CalculatePathLossWithGain(transmitter_, receiver_position, records_, result_, recorder_)) {
		std::cout << " ------------------------------- \n";
		std::cout << "Receiver Position: " << glm::to_string(receiver_position) << std::endl;
		std::cout << "Transmitter Position: " << glm::to_string(transmitter_positon) << std::endl;
		std::cout << "Direct Distance: " << glm::distance(transmitter_positon, receiver_position) << std::endl;
		std::cout << "Direct Path Loss: " << result_.direct_path_loss_in_linear << ", dB: " << 10.0 * log10(result_.direct_path_loss_in_linear) << std::endl;
		std::cout << "Reflect Path Loss: " << result_.reflection_loss_in_linear << ", dB: " << 10.0 * log10(result_.reflection_loss_in_linear) << std::endl;
		std::cout << "Diffraction Loss: " << result_.diffraction_loss_in_linear << ", dB: " << 10.0 * log10(result_.diffraction_loss_in_linear) << std::endl;
		std::cout << "Total Loss: " << result_.total_loss << " dB\n";
		std::cout << " ------------------------------- \n";
	}*/
}
void Receiver::UpdateAndVisualize()
{
	const glm::vec3 receiver_position = transform_.position;
	const glm::vec3 transmitter_positon = transmitter_->GetPosition();
	UpdateResult();
	Clear();
	ray_tracer_->GetDrawComponents(transmitter_positon, receiver_position, records_, objects_);
}
void Receiver::Reset()
{
	result_.is_valid = false;
	move_speed_ = 10.0f;
}

void Receiver::MoveTo(const glm::vec3 position) {
	transform_.position = position;
	UpdateResult();
}

void Receiver::Move(Direction direction,float delta_time) {
	float distance = delta_time * move_speed_;
	glm::vec3& rotation = transform_.rotation;

	glm::mat4 trans = glm::rotate(glm::mat4(1.0f), -transform_.rotation.x, glm::vec3(0.0f, 1.0f, 0.0f));
	trans = glm::rotate(trans, -transform_.rotation.y, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec3 front_direction;
	glm::vec3 up_direction;
	glm::vec3 right_direction;

	constexpr glm::vec3 x_axis = glm::vec3(1.0f, 0.0f, 0.0f);
	constexpr glm::vec3 y_axis = glm::vec3(0.0f, 1.0f, 0.0f);

	switch (direction) {
	case Direction::kForward:
		front_direction = glm::vec3(trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		transform_.position += front_direction * distance;
		break;
	case Direction::kBackward:
		front_direction = glm::vec3(trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		transform_.position -= front_direction * distance;
		break;
	case Direction::kRight:
		front_direction = glm::vec3(trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		up_direction = glm::vec3(trans * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		right_direction = glm::cross(front_direction, up_direction);
		transform_.position += right_direction * distance;
		break;
	case Direction::kLeft:
		front_direction = glm::vec3(trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		up_direction = glm::vec3(trans * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		right_direction = glm::cross(front_direction, up_direction);
		transform_.position -= right_direction * distance;
		break;
	case Direction::kUp:
		up_direction = glm::vec3(trans * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		transform_.position += up_direction * distance;
		break;
	case Direction::kDown:
		up_direction = glm::vec3(trans * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		transform_.position -= up_direction * distance;
		break;
	};
	UpdateResult();
}

void Receiver::Clear()
{
	for (auto * object : objects_)
		free(object);
	objects_.clear();
}

void Receiver::DrawObjects(Camera * main_camera)
{
	for (auto object : objects_)
		object->DrawObject(main_camera);
}
