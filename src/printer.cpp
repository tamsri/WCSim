#include "printer.hpp"

Printer::Printer(RayTracer* ray_tracer):ray_tracer_(ray_tracer)
{
}

void Printer::Print(std::string output_path, glm::vec3 transmitter_location)
{

}

void Printer::ScanMap()
{
	/// Scanner's info
	unsigned int resolution_x = 1000;
	unsigned int resolution_z = 1000;
	float min_x = -100.0f; //the minimum x to print
	float max_x = +100.0f; //the maximum x to print
	float min_z = -100.0f; //the minimum z to print
	float max_z = +100.0f; //the maximum z y to print 
	float y = 1.5f; // the height of receiver at y

	/// Transmitter's info
	const float x_step = (max_x - min_x) / (float)resolution_x;
	const float z_step = (max_z - min_z) / (float)resolution_z;
	for (float x = min_x; x <= max_x; x += x_step) {
		std::vector<float> z_map;
		for (float z = min_z; z <= max_z; z += z_step) {
		
		}
	}
}
