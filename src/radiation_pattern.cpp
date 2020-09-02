#include "radiation_pattern.hpp"

#include<iostream>
#include<fstream>

RadiationPattern::RadiationPattern(std::string pattern_file_path) {
	std::ifstream input_file_stream(pattern_file_path);
	if (input_file_stream.is_open()) {
		std::cout << "Reading Radiation Pattern file" << std::endl;
		std::string line;
		while(getline(input_file_stream, line)) {
			if (line[0] == '#' || line[0] == '*')continue;
			float theta, phi, total_gain;
			float reeth, imeth, rephi, imphi, gth, gphi;
			input_file_stream >> theta >> phi >> reeth >> imeth >> rephi >> imphi >> gth >> gphi >> total_gain;
			pattern_[std::make_pair(theta, phi)] = total_gain;
			/*std::cout  << theta << " " 
						<< phi << " " 
						/*<< reeth << " "
				<< imeth << " "
				<< rephi << " "
				<< phi << " "
				<< imphi << " "
				<< gth << " "
				<< gphi << " "*/
			//	<< total_gain << std::endl;*/
		}
		std::cout << "Radiation Pattern Reading Completed." << std::endl;
		input_file_stream.close();
	}
	else {
		std::cout << "Couldn't open radiation pattern file." << std::endl;
	}
}