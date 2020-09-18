#ifndef PRINTER_H
#define PRINTER_H

#include<vector>
#include<string>
#include<glm/glm.hpp>

class Transmitter;
class RayTracer;
struct Result;

class Printer {
public:
	Printer(RayTracer* ray_tracer);

	void Print(const std::string & output_path, glm::vec3 transmitter_location, float frequency, float scanning_height);

private:
	void ScanMap(glm::vec3 transmitter_position, float frequency, float scanning_heigh, float x_resolution, float z_resolution);
	glm::vec3 GetHeatColor(float min_value, float max_value, float value);
	RayTracer* ray_tracer_;
	float min_value_;
	float max_value_;
	Transmitter* transmitter_;
	std::vector<std::vector<Result>> print_map_;
};
#endif