#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <vector>

#include "transform.hpp"

class Camera;
class Ray;
class Point;
class RadiationPattern;
class RayTracer;
class Object;

class Transmitter {
public:
	Transmitter(Transform transform, float frequency, RayTracer * ray_tracer);

	void DrawRadiationPattern(Camera * camera);
	void AssignRadiationPattern(RadiationPattern* pattern);
	float GetFrequency();
	Point * GetPoint();
private:
	float transmitter_power_output_;
	float frequency_;
	Transform transform_;
	RadiationPattern * current_pattern;
	RayTracer* ray_tracer_;
	Point* current_point_;
	
	// Visualization
	std::vector<Ray* > rays_;
};
#endif
