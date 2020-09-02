#include "receiver.hpp"
#include "ray_tracer.hpp"
#include "transmitter.hpp"

Receiver::Receiver(Transform transform, RayTracer * ray_tracer, Transmitter * transmitter)
{
	transform_ = transform;
	ray_tracer_ = ray_tracer_;
	current_point_ = ray_tracer_->InitializeOrCallPoint(transform_.position);
}

void Receiver::Update()
{
	if (velocity_ != glm::vec3(0.0f) ) return;
	
	transform_.position += velocity_;
	current_point_ = ray_tracer_->InitializeOrCallPoint(transform_.position);
	ray_tracer_->CalculatePathLoss(transmitter_->GetPoint(), current_point_, total_path_loss_dB, transmitter_->GetFrequency() );
}
void Receiver::Move(glm::vec3 step) {
	transform_.position += step;
}