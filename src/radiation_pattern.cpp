#include "radiation_pattern.hpp"

#include<iostream>
#include<fstream>

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