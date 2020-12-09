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
    float direct_loss;
    float delay;
    float tx_gain;
    float rx_gain;
};

struct ReflectionResult{
    float reflection_loss;
    float delay;
    float tx_gain;
    float rx_gain;
};

struct DiffractionResult{
    float diffraction_loss;
    float delay;
    float tx_gain;
    float rx_gain;
};
struct Result {
	// Validation of Result.
    bool is_valid = false;
    // Is line-of-sight?
    bool is_los = false;
	// Direct Path Result.
    DirectResult direct {0.0f,0.0f,0.0f,0.0f};
    // Reflection Results
    std::vector<ReflectionResult> reflections;
    // Diffraction Result
    DiffractionResult diffraction{ 0.0f,0.0f,0.0f,0.0f };
	// Transmitting Power
	float transmit_power = 0.0f; // Unit: dBm
    // Total Loss
    float total_attenuation = 0.0f; // Unit: dB
    // Total Received Power
	float total_received_power = 0.0f; // Unit: dBm
};

struct Record {
	Record(RecordType record_type);
	Record(RecordType record_type, std::vector<glm::vec3> record_data);
	RecordType type;
	std::vector<glm::vec3> data;
};


#endif // !RECORD_H_