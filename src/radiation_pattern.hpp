#ifndef RADIATION_PATTERN_H
#define RADIATION_PATTERN_H
#include <map>
#include <utility>
#include <string>
class RadiationPattern {
public:
	RadiationPattern(std::string pattern_file_path);

	std::map<std::pair<float, float>, float> pattern_;
	float min_gain_;
	float max_gain_;
};
#endif // !RADIATION_PATTERN_H