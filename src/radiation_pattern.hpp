#ifndef RADIATION_PATTERN_H
#define RADIATION_PATTERN_H
#include <map>
#include <utility>
#include <string>
#include <set>

class Pattern;
class Camera;
class RadiationPattern {
public:
	RadiationPattern(std::string pattern_file_path);

	// Visualisaton
	void GetPattern();
	void DrawPattern(Camera * camera);

	std::map<float, std::map<float, float>> pattern_;
	float GetGain(float theta, float phi);
	float min_gain_;
	float max_gain_;

	Pattern* pattern_object_;
};
#endif // !RADIATION_PATTERN_H