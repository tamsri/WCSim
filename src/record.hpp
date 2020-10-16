#ifndef RECORD_H_
#define RECORD_H_

#include <vector>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include <unordered_map>
#include <map>

class Triangle;

enum class RecordType : int {
	kDirect = 0,
	kReflect,
	kEdgeDiffraction
};

struct DirectResult{
    float receive_power;
    float delay;
    float tx_gain;
    float rx_gain;
};

struct ReflectionResult{
    std::vector<float> receive_powers;
    std::vector<float> delays;
    std::vector<float> tx_gains;
    std::vector<float> rx_gains;
};

struct DiffractionResult{
    float receive_power;
    float delay;
    float tx_gain;
    float rx_gain;
};
struct Result {
	// Validation of Result.
    bool is_valid;
	// Direct Path Result.
	DirectResult direct;
    // Reflection Results
	ReflectionResult reflection;
    // Diffraction Result
	DiffractionResult diffraction;
    // Total Loss
	float total_received_power;

};

struct Record {
	Record(RecordType record_type);
	Record(RecordType record_type, std::vector<glm::vec3> record_data);
	RecordType type;
	std::vector<glm::vec3> data;
};

struct Point {
	Point(glm::vec3 point_position);
	glm::vec3 position;

	std::map<const Triangle*, bool> hit_triangles;
};

#endif // !RECORD_H_