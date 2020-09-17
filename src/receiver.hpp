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
struct Result;

class Receiver {

public:

	Receiver(Transform transform, RayTracer * ray_tracer, Transmitter * transmitter);

	Result GetResult() const;
	glm::vec3 GetPosition() const;
	// Actions
	void MoveTo(const glm::vec3 position);
	void Move(Direction direction, float delta_time);

	// Visualization
	void Update();
	void Reset();
 

	void Clear();

	void DrawObjects(Camera* main_camera);
private:

	Result result_;

	// Variables
	Transform transform_;
	glm::vec3 velocity_;
	float move_speed_;
	glm::vec3 front_direction_;
	glm::vec3 up_direction_;

	// Ray Tracer
	RayTracer * ray_tracer_;
	Transmitter* transmitter_;
	
	// Visualisation
	std::vector<Object*> objects_;
};
#endif //!RECEIVER_H