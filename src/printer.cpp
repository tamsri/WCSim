#include "printer.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include <chrono> 

#include "glm/gtx/string_cast.hpp"

#include "record.hpp"
#include "ray_tracer.hpp"

Printer::Printer(RayTracer* ray_tracer):ray_tracer_(ray_tracer)
{
	max_value_ = -100.0f;
	min_value_ = -60.0f;
}

void Printer::Print(const std::string & output_path, glm::vec3 transmitter_location, float frequency , float scanning_heigh)
{
	
	std::ofstream output_file(output_path);
	if (!output_file.is_open()) {
		std::cout << "Cannot print the map" << std::endl;
		return;
	}
	ScanMap(transmitter_location, frequency, scanning_heigh);

	int x_width = 50;
	int z_width = 50;
	output_file << "P3\n" << x_width << " " << z_width << "\n255\n";
	for (auto& x_row : print_map_)
		for (auto& result : x_row) {
			glm::vec3 color;  
			if (result.is_valid) {
				color = GetHeatColor(min_value_, max_value_, result.total_loss);
				output_file << int(color.x * 256.0f) << " " << int(color.y * 256.0f) << " " << int(color.z * 256.0f) << "\n";

			}
			else {
				output_file << 0 << " " << 0 << " " << 0 << "\n";
			}
		}
	std::cout << " Completed printing" << std::endl;
	return;
}

void Printer::ScanMap(glm::vec3 transmitter_position, float frequency, float scanning_heigh)
{
	/// Scanner's info
	unsigned int resolution_x = 50;
	unsigned int resolution_z = 50;
	float min_x = -100.0f; //the minimum x to print
	float max_x = +100.0f; //the maximum x to print
	float min_z = -100.0f; //the minimum z to print
	float max_z = +100.0f; //the maximum z y to print 

	const float x_step = (max_x - min_x) / (float)resolution_x;
	const float z_step = (max_z - min_z) / (float)resolution_z;
	for (float z = min_z; z <= max_z; z += z_step) {
		std::vector<Result> z_map;
		for (float x = min_z; x <= max_z; x += x_step) {
			glm::vec3 receiver_position = glm::vec3(x, scanning_heigh, z);
			std::vector<Record> records;

			using namespace std::chrono;

			auto trace_start = high_resolution_clock::now();
			ray_tracer_->Trace(transmitter_position, receiver_position, records);
			

			Result result;
			ray_tracer_->CalculatePathLoss(transmitter_position, receiver_position, frequency, records, result);
			if (result.is_valid) {
				min_value_ = std::min({ result.total_loss, min_value_ });
				max_value_ = std::max({ result.total_loss, max_value_});
			}
			z_map.push_back(result);
			auto trace_stop = high_resolution_clock::now();
			auto trace_duration = duration_cast<microseconds>(trace_stop - trace_start).count();
			std::cout << "the point takes: " << trace_duration << "ms\n";
		}
		print_map_.push_back(z_map);
	}
}

glm::vec3 Printer::GetHeatColor(float min_value, float max_value, float value)
{
	glm::vec3 color; // assume x is reg, y is green, z is blue;
	float n_value = (value-min_value) / (max_value - min_value); // nomrlaized value

	color.x = std::max( 2.0f * n_value -1, 0.0f);
	color.z = std::max(-2.0f * n_value + 1.0f, 0.0f);
	color.y = 1.0f - color.x - color.z;
	//std::cout << "n_value: " << n_value << " min: " << min_value << "max: " << max_value <<std::endl;
	//std::cout << "value: " << value << std::endl;
	//std::cout << "color: " << glm::to_string(color) << std::endl;
	return glm::vec3(color);
}
