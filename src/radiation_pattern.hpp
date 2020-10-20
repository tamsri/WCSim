#ifndef RADIATION_PATTERN_H
#define RADIATION_PATTERN_H
#include <map>
#include <utility>
#include <string>
#include <set>

class Pattern;
class Camera;
class PolygonMesh;
struct Transform;

class RadiationPattern {
public:
	RadiationPattern(std::string pattern_file_path);

	// Visualisaton
	void PrepareVisualPattern();
	void DrawPattern(Camera * camera, Transform & transform);

	std::map<float, std::map<float, float>> pattern_;
	float GetGain(float theta, float phi);
	float min_gain_;
	float max_gain_;

	PolygonMesh * pattern_shape_;
};
#endif // !RADIATION_PATTERN_H