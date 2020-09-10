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

Point* Receiver::GetPoint()
{
	return current_point_;
}

Result Receiver::GetResult()
{
	return result_;
}

void Receiver::Update()
{
	Point* transmitter_point = transmitter_->GetPoint();
	float transmitter_frequency = transmitter_->GetFrequency();

	//if (!ray_tracer_->store_points && current_point_ != nullptr) delete current_point_;
	//ray_tracer_->ClearPoint(current_point_);
	current_point_ = ray_tracer_->InitializeOrCallPoint(transform_.position);

	ray_tracer_->Trace(transmitter_point, current_point_);
	if (ray_tracer_->CalculatePathLoss(transmitter_, this, result_)) {
		result_.is_valid = true;
		if (ray_tracer_->print_each_ == true) {
			std::cout << " ------------------------------- \n";
			std::cout << "Receiver Position: " << glm::to_string(current_point_->position) << std::endl;
			std::cout << "Transmitter Position: " << glm::to_string(transmitter_point->position) << std::endl;
			std::cout << "Direct Distance: " << glm::distance(current_point_->position, transmitter_point->position) << std::endl;
			std::cout << "Direct Path Loss: " << result_.direct_path_loss_in_linear << ", dB: " << 10.0 * log10(result_.direct_path_loss_in_linear) << std::endl;
			std::cout << "Reflect Path Loss: " << result_.reflection_loss_in_linear << ", dB: " << 10.0 * log10(result_.reflection_loss_in_linear) << std::endl;
			std::cout << "Diffraction Loss: " << result_.diffraction_loss_in_linear << ", dB: " << 10.0 * log10(result_.diffraction_loss_in_linear) << std::endl;
			std::cout << "Total Loss: " << result_.total_loss << " dB\n";
			std::cout << " ------------------------------- \n";
		}
	}
	else {
		result_.is_valid = false;
	}


	//Clear Visualisation
	Clear();
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

void Receiver::Clear()
{
	for (auto * object : objects_)
		delete object;
	objects_.clear();
}

void Receiver::CalculateReceivePower()
{
}

void Receiver::DrawObjects(Camera * main_camera)
{
	for (auto object : objects_)
		object->DrawObject(main_camera);
}
