#include "receiver.hpp"

#include <iostream>


#include "object.hpp"

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

void Receiver::Update()
{
	Point* transmitter_point = transmitter_->GetPoint();
	float transmitter_frequency = transmitter_->GetFrequency();

	current_point_ = ray_tracer_->InitializeOrCallPoint(transform_.position);
	ray_tracer_->Trace(transmitter_point, current_point_);
	total_path_loss_dB_ = 0.0f;
	ray_tracer_->CalculatePathLoss(transmitter_point, current_point_, total_path_loss_dB_, transmitter_frequency); // implement if
	std::cout << "Receiver at (" << current_point_->position.x << ", " << current_point_->position.y << ", " << current_point_->position.z << ")\n";
	std::cout << "Trams,otter at (" << transmitter_point->position.x << ", " << transmitter_point->position.y << ", " << transmitter_point->position.z << ")\n";

	std::cout << "Frequency: " << transmitter_frequency << std::endl;
	std::cout << "Total Path Loss [dB]: " << total_path_loss_dB_ << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	objects_.clear();
	ray_tracer_->GetDrawPointsComponent(transmitter_point, current_point_, objects_);
}
void Receiver::Reset()
{
	move_speed_ = 10.0f;
	front_direction_ = glm::vec3(1.0f, 0.0f, 0.0);
	up_direction_ = glm::vec3(0.0f, 1.0f, 0.0);
}
void Receiver::Move(glm::vec3 step) {
	transform_.position += step;
	Update();
}

void Receiver::Move(Direction direction,float delta_time) {
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

void Receiver::DrawObjects(Camera * main_camera)
{
	for (auto object : objects_)
		object->DrawObject(main_camera);
}
