#ifndef RECEIVER_H
#define RECEIVER_H

#include <vector>
#include <glm/glm.hpp>
#include "transform.hpp"

#include "record.hpp" 
#include "camera.hpp"

class RayTracer;
class Transmitter;
class Object;


class Receiver {

public:

	Receiver(Transform transform, RayTracer * ray_tracer, Transmitter * transmitter);

	// Variables
	Transform transform_;
	glm::vec3 velocity_;
	float total_path_loss_dB_;

	float move_speed_;
	glm::vec3 front_direction_;
	glm::vec3 up_direction_;
	// Visualization
	void Update();
	void Reset();
	void Move(glm::vec3 step);
	void Move(Direction direction, float delta_time);
	// Calculation

	// Ray Tracer
	Point* current_point_;
	RayTracer * ray_tracer_;
	Transmitter* transmitter_;
	
	// Visualisation
	std::vector<Object*> objects_;
	void DrawObjects(Camera* main_camera);
};
#endif //!RECEIVER_H