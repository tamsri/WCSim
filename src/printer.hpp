#ifndef PRINTER_H
#define PRINTER_H

#include<vector>
#include<string>
#include<glm/glm.hpp>

class RayTracer;
class Printer {
public:
	Printer(RayTracer* ray_tracer);

	void Print(std::string output_path, glm::vec3 transmitter_location );
	void ScanMap();
	
	glm::vec3 GetHeatColor(float min_value,float max_value,float value);

private:
	RayTracer* ray_tracer_;
	float min_value_;
	float max_value_;
	std::vector<std::vector<float>> print_map_;
};
#endif