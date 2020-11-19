#include "receiver.hpp"

#include "object.hpp"
#include "shader.hpp"
#include "ray_tracer.hpp"
#include "transmitter.hpp"
#include "cube.hpp"

unsigned int Receiver::global_id_ = 0;
Receiver::Receiver(Transform transform, RayTracer* ray_tracer):	id_(++global_id_),
														transform_(transform),
														ray_tracer_(ray_tracer),
														transmitter_(nullptr),
														velocity_(0),
														move_speed_(0),
														object_(nullptr)
{
	Reset();
}

Receiver::Receiver(Transform transform, RayTracer * ray_tracer, Transmitter * transmitter): 
	id_(global_id_++),
	transform_(transform),	
	ray_tracer_(ray_tracer),
	transmitter_(transmitter),
    velocity_(0),
    move_speed_(0),
	object_(nullptr)
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
	UpdateResult();
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

Result Receiver::GetResult() const
{
	return result_;
}

glm::vec3 Receiver::GetPosition() const
{
	return transform_.position;
}

void Receiver::UpdateResult()
{
	if (transmitter_ == nullptr) return;
	const glm::vec3 rx_pos = transform_.position;
	const glm::vec3 tx_pos = transmitter_->GetPosition();
	records_.clear();
	ray_tracer_->Trace(tx_pos, rx_pos, records_);
	ray_tracer_->CalculatePathLoss( transmitter_, this, records_, result_);
}
void Receiver::UpdateVisualRayComponents()
{
    Clear();
    if(transmitter_ == nullptr) return;
	const auto tx_pos = transform_.position;
	const auto rx_pos = transmitter_->GetPosition();
	ray_tracer_->GetDrawComponents(tx_pos, rx_pos, records_, rays_);
}
void Receiver::Reset()
{
	result_.is_valid = false;
	move_speed_ = 10.0f;
}

void Receiver::MoveTo(const glm::vec3 position) {
	if (object_ == nullptr) return;
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
	case (Direction::kForward):
		front_direction = glm::vec3(trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		transform_.position += front_direction * distance;
		break;
	case (Direction::kBackward):
		front_direction = glm::vec3(trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		transform_.position -= front_direction * distance;
		break;
	case (Direction::kRight):
		front_direction = glm::vec3(trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		up_direction = glm::vec3(trans * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		right_direction = glm::cross(front_direction, up_direction);
		transform_.position += right_direction * distance;
		break;
	case (Direction::kLeft):
		front_direction = glm::vec3(trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		up_direction = glm::vec3(trans * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		right_direction = glm::cross(front_direction, up_direction);
		transform_.position -= right_direction * distance;
		break;
	case (Direction::kUp):
		up_direction = glm::vec3(trans * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		transform_.position += up_direction * distance;
		break;
	case (Direction::kDown):
		up_direction = glm::vec3(trans * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		transform_.position -= up_direction * distance;
		break;
	}
	UpdateResult();
}


float Receiver::GetReceiverGain(const glm::vec3 & near_rx_position) const {
    // TODO: Implement Receiver Gain.
    return 0.0f;
}

void Receiver::Clear() {
    for(auto ray : rays_)
        delete ray;
    rays_.clear();
}

void Receiver::DrawObjects(Camera *main_camera) {
    object_->DrawObject(main_camera);
    for(auto ray : rays_)
        ray->DrawObject(main_camera);
}

void Receiver::VisualUpdate(){
	if (object_ == nullptr) return;
	object_->MoveTo(transform_.position);
    UpdateVisualRayComponents();
}

void Receiver::InitializeVisualObject(Shader * shader){
    object_ = new Cube(transform_, shader);
}

bool Receiver::IsInitializedObject()
{
	return object_ != nullptr;
}

Receiver::~Receiver() {
    // If delete object_.
    delete object_;
}
