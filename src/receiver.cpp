#include "receiver.hpp"

#include <iostream>


#include "object.hpp"
#include "glm/gtx/string_cast.hpp"
#include "ray_tracer.hpp"
#include "transmitter.hpp"



Receiver::Receiver(Transform transform, RayTracer * ray_tracer, Transmitter * transmitter): 
	transform_(transform),	
	ray_tracer_(ray_tracer),
	transmitter_(transmitter)
{
	Reset();
	Update();
}

Result Receiver::GetResult() const
{
	return result_;
}

glm::vec3 Receiver::GetPosition() const
{
	return glm::vec3(transform_.position);
}

void Receiver::Update()
{

	const glm::vec3 receiver_position = transform_.position;
	const glm::vec3 transmitter_positon = transmitter_->GetPosition();
	float transmitter_frequency = transmitter_->GetFrequency();


	std::vector<Record> records;
	ray_tracer_->Trace(transmitter_positon, receiver_position, records);
	
	if (ray_tracer_->CalculatePathLossWithGain(transmitter_, receiver_position, records, result_)) {
		std::cout << " ------------------------------- \n";
		std::cout << "Receiver Position: " << glm::to_string(receiver_position) << std::endl;
		std::cout << "Transmitter Position: " << glm::to_string(transmitter_positon) << std::endl;
		std::cout << "Direct Distance: " << glm::distance(transmitter_positon, receiver_position) << std::endl;
		std::cout << "Direct Path Loss: " << result_.direct_path_loss_in_linear << ", dB: " << 10.0 * log10(result_.direct_path_loss_in_linear) << std::endl;
		std::cout << "Reflect Path Loss: " << result_.reflection_loss_in_linear << ", dB: " << 10.0 * log10(result_.reflection_loss_in_linear) << std::endl;
		std::cout << "Diffraction Loss: " << result_.diffraction_loss_in_linear << ", dB: " << 10.0 * log10(result_.diffraction_loss_in_linear) << std::endl;
		std::cout << "Total Loss: " << result_.total_loss << " dB\n";
		std::cout << " ------------------------------- \n";
	}

	//Clear Visualisation
	Clear();
	ray_tracer_->GetDrawComponents(transmitter_positon, receiver_position, records ,objects_);
}
void Receiver::Reset()
{
	move_speed_ = 10.0f;
	front_direction_ = glm::vec3(1.0f, 0.0f, 0.0);
	up_direction_ = glm::vec3(0.0f, 1.0f, 0.0);
}

void Receiver::MoveTo(const glm::vec3 position) {
	transform_.position = position;
	Update();
}

void Receiver::Move(Direction direction,float delta_time) {
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
