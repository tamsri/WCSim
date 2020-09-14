#include "radiation_pattern.hpp"
#include<iostream>
#include<fstream>

#include <glm/glm.hpp>


RadiationPattern::RadiationPattern(std::string pattern_file_path) {
	std::ifstream input_file_stream(pattern_file_path);
	if (input_file_stream.is_open()) {
		std::cout << "Reading Radiation Pattern file" << std::endl;
		std::string line;
		min_gain_ = 0.0f;
		max_gain_ = 0.0f;
		while(getline(input_file_stream, line)) {
			if (line[0] == '#' || line[0] == '*')continue;
			float theta, phi, total_gain;
			float reeth, imeth, rephi, imphi, gth, gphi;
			input_file_stream >> phi >> theta >> reeth >> imeth >> rephi >> imphi >> gth >> gphi >> total_gain;
			pattern_[theta].insert({ phi, total_gain });


			if (total_gain < min_gain_) min_gain_ = total_gain;
			if (total_gain > max_gain_) max_gain_ = total_gain;
		}
		std::cout << "Radiation Pattern Reading Completed." << std::endl;
		input_file_stream.close();
	}
	else {
		std::cout << "Couldn't open radiation pattern file." << std::endl;
	}
}

float RadiationPattern::GetGain(float theta, float phi)
{
	//std::cout << "getting gain. " << std::endl;
	float theta_deg = glm::degrees(theta);
	float phi_deg = glm::degrees(phi);
	if (theta_deg >= 360.0f || phi_deg >= 360.0f) {
		//std::cout << "!!! theta:" << theta_deg << " phi:" << phi_deg << std::endl;

		theta_deg = (int)theta_deg % 360;
		phi_deg = (int)phi_deg % 360;

	}

	/*if (phi_deg >= 180.0f) {
		//phi_deg = phi_deg - 180.0f;// Out of range, flip the angle
		//theta_deg = 360.0f - theta_deg;
		return min_gain_;
	}*/

	auto& phi_value = (*pattern_.lower_bound(theta_deg)).second;
	auto gain = (*phi_value.lower_bound(phi_deg)).second;
	//std::cout << "theta:" << theta_deg << " phi:" << phi_deg << " total gain: " << gain << std::endl;

	// Debug the problem
	return gain;
}
