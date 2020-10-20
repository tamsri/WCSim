#include "printer.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include <chrono> 

#include "glm/gtx/string_cast.hpp"

#include "record.hpp"
#include "ray_tracer.hpp"
#include "transmitter.hpp"
#include "receiver.hpp"

using namespace std::chrono;

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

	int x_width = 60;
	int z_width = 60;
	ScanMap(transmitter_location, frequency, scanning_heigh, x_width, z_width);

	output_file << "P3\n" << x_width << " " << z_width << "\n255\n";
	for (auto& x_row : print_map_)
		for (auto& result : x_row) {
			glm::vec3 color;  
			if (result.is_valid) {
				color = GetHeatColor(/*min_value_*/ -130.0f, max_value_, result.total_received_power);
				output_file << int(color.x * 256.0f) << " " << int(color.y * 256.0f) << " " << int(color.z * 256.0f) << "\n";
			}
			else {
				output_file << 0 << " " << 0 << " " << 0 << "\n";
			}
		}
	std::cout << "min: " << min_value_  << " max: " << max_value_ << std::endl;
	std::cout << " Completed printing" << std::endl;
	output_file.close();
	return;
}
void Printer::TestPrint(const std::string& output_path) {

	std::ofstream output_file(output_path);
	if (!output_file.is_open()) {
		std::cout << "Cannot print the map" << std::endl;
		return;
	}
	std::cout << "Start Testing the printer heat map\n";
	const int x_width = 500;
	const int y_width = 500;
	output_file << "P3\n" << x_width << " " << y_width << "\n255\n";
	for (auto i = 0; i < x_width; ++i) {
		for (auto j = 0; j < y_width; ++j) {
			glm::vec3 color;
			color = GetHeatColor(0, x_width, i);
			output_file << int(color.x * 256.0f) << " " << int(color.y * 256.0f) << " " << int(color.z * 256.0f) << "\n";
		}
	}


};
void Printer::ScanMap(glm::vec3 transmitter_position, float frequency, float scanning_heigh, float x_resolution, float z_resolution)
{
	/// Scanner's info
	float min_x = -100.0f; //the minimum x to print
	float max_x = +100.0f; //the maximum x to print
	float min_z = -100.0f; //the minimum z to print
	float max_z = +100.0f; //the maximum z y to print 

	//transmitter_ = new Transmitter(Transform{ transmitter_position, glm::vec3(0.0f), glm::vec3(0.0f) }, frequency, ray_tracer_);
	const float x_step = (max_x - min_x) / (float)x_resolution;
	const float z_step = (max_z - min_z) / (float)z_resolution;
	auto print_start = high_resolution_clock::now();

	Transmitter * tx = new Transmitter({transmitter_position, glm::vec3(0.0f),
                                     glm::vec3(1.0f)}, frequency, 0, ray_tracer_);
	Receiver * rx = new Receiver({transmitter_position, glm::vec3(0.0f), glm::vec3(1.0) }, ray_tracer_);
	tx->ConnectAReceiver(rx);


	std::vector<std::pair<glm::vec3, std::vector<Record>>> recordss;
	std::vector<std::thread> threads;
	for (float z = min_z; z < max_z; z = z + z_step) {
		for (float x = min_x; x < max_x; x = x + x_step) {
		        std::vector<Record> records;
				glm::vec3 receiver_position = glm::vec3(x , scanning_heigh, z );

				std::thread trace_thread(  &RayTracer::Trace, ray_tracer_,
                                            transmitter_position, receiver_position,
                                            std::ref(records));
                threads.push_back(std::move(trace_thread));
                recordss.emplace_back(receiver_position, records);
			}
		}
	for(auto & thread: threads){
	    if(thread.joinable()) thread.join();
	}
	/// TODO: Calculate the record.
}

glm::vec3 Printer::GetHeatColor(float min_value, float max_value, float value)
{
	if (value > max_value) return glm::vec3(0.9f, 0.0f, 0.0f);
	if (value < min_value) return glm::vec3(0.0f, 0.0f, 0.9f);
	glm::vec3 color; // assume x is reg, y is green, z is blue;
	float n_value = (value-min_value) / (max_value - min_value); // nomrlaized value

	// Simple Linear Color
	color.z = std::max(2.0f * (-n_value + 0.5f), 0.0f);
	color.x = std::max(2.0f * ( n_value - 0.5f), 0.0f);
	
	// Sigmoid Function Implementation
	//float sl = 0.05f; // slope
	//color.x = 1.0f / (1.0f + exp((n_value - 0.25f)/sl));
	//color.z = 1.0f / (1.0f + exp((-n_value + 0.75f)/sl));
	
	color.y = 1.0 - color.x - color.z;
	color = glm::normalize(color);
	return glm::vec3(color);
}
