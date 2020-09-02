#ifndef RECEIVER_H
#define RECEIVER_H

#include <glm/glm.hpp>
#include "transform.hpp"

class Point;
class RayTracer;
class Transmitter;

class Receiver {

public:

	Receiver(Transform transform, RayTracer * ray_tracer, Transmitter * transmitter);

private:
	
	Transform transform_;
	glm::vec3 velocity_;

	void Update();
	void Move(glm::vec3 step);
	// Visualization

	// Calculation
	float total_path_loss_dB;

	// Ray Tracer
	Point* current_point_;
	RayTracer * ray_tracer_;
	Transmitter* transmitter_;
};
#endif //!RECEIVER_H