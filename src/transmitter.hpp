#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <vector>

#include "transform.hpp"

class Camera;
class Shader;
class Ray;

class Transmitter {
public:
	Transmitter(Transform transform);

	void DrawRadiationPattern(Camera * camera);
private:
	float transmitter_power_output_;
	Transform transform_;
	//std::vector<std::vector<unsigned int>> radiation_pattern_;
	std::vector<Ray * > rays_;
};
#endif
