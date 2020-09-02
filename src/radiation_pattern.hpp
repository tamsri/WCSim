#ifndef RADIATION_PATTERN_H
#define RADIATION_PATTERN_H
#include<map>
#include<utility>

class RadiationPattern {
	RadiationPattern(std::string pattern_file_path);

	std::map<std::pair<float, float>, float> pattern_;
};
#endif // !RADIATION_PATTERN_H