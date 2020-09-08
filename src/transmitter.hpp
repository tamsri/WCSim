#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <vector>

#include "transform.hpp"
#include <glm/matrix.hpp>
#include "camera.hpp"

class Camera;
class Ray;
class Point;
class RadiationPattern;
class RayTracer;
class Object;
class Receiver;
struct Result;

class Transmitter {
public:
	Transmitter(Transform transform, float frequency, RayTracer * ray_tracer);

	// Actions
	void AddReceiver(Receiver* receiver);
	void DrawRadiationPattern(Camera * camera);
	void AssignRadiationPattern(RadiationPattern* pattern);
	float GetFrequency();
	Point * GetPoint();
	float GetTransmitterGain(glm::vec3 near_tx_position);

	// Movement
	void Move(const Direction direction, float delta_time);
	void Rotate(const Rotation rotation, float delta_time);
	
	void Update();
	void Reset();

	// Visualisation
	void DrawObjects(Camera * camera);
private:
	
	// Variables
	float transmitter_power_output_;
	float frequency_;
	float move_speed_;
	float rotation_speed_;
	glm::vec3 front_direction_, up_direction_;

	Transform transform_;
	RadiationPattern * current_pattern;
	RayTracer* ray_tracer_;
	Point* current_point_;
	
	std::vector<Receiver* > receivers_;
	std::vector<Result> receiver_results;
	// Visualization
	std::vector<Ray* > rays_;
};
#endif
