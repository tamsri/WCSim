#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <vector>

#include "transform.hpp"

class Camera;
class Ray;
class Point;
class RadiationPattern;

class Transmitter {
public:
	Transmitter(Transform transform, float frequency);

	void DrawRadiationPattern(Camera * camera);
	float GetFrequency();
	Point * GetPoint();
private:
	float transmitter_power_output_;
	float frequency_;
	Transform transform_;
	RadiationPattern * current_pattern;
	Point* current_point_;
	std::vector<Ray * > rays_;
};
#endif
