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
	Transmitter(Transform transform, 
				float frequency, 
				RayTracer * ray_tracer);

	// Actions
	void AddReceiver(Receiver* receiver);
	void DrawRadiationPattern(Camera * camera);
	void AssignRadiationPattern(RadiationPattern* pattern);


	float GetFrequency();
	float GetTransmitterGain(glm::vec3 near_tx_position);
	Receiver* GetReceiver(unsigned int index);
	glm::vec3 GetPosition();
	// Movement
	void Move(const Direction direction, float delta_time);
	void Rotate(const Direction rotation, float delta_time);
	void ToggleDisplay();

	void Update();
	void UpdateRadiationPattern();
	void Reset();
	void Clear();
	// Visualisation
	void DrawObjects(Camera * camera);
private:
	
	// Variables
	float transmitter_power_output_;
	float frequency_;
	float move_speed_;
	float rotation_speed_;
	glm::vec3 front_direction_, up_direction_;

	bool display_pattern_;
	Transform transform_;
	RadiationPattern * current_pattern_;
	RayTracer* ray_tracer_;
	Point* current_point_;
	
	std::vector<Receiver* > receivers_;
	std::vector<Result> receiver_results;
	// Visualization
	std::vector<Object* > objects_;
};
#endif
