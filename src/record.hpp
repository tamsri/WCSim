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
    std::vector<float> relection_losses;
    std::vector<float> delays;
    std::vector<float> tx_gains;
    std::vector<float> rx_gains;
};

struct DiffractionResult{
    float diffraction_loss;
    float delay;
    float tx_gain;
    float rx_gain;
};
struct Result {
	// Validation of Result.
    bool is_valid;
    // Is line-of-sight?
    bool is_los;
	// Direct Path Result.
	DirectResult direct;
    // Reflection Results
	ReflectionResult reflection;
    // Diffraction Result
	DiffractionResult diffraction;
	// Transmitting Power
	float transmit_power; // Unit: dBm
    // Total Loss
    float total_attenuation; // Unit: dB
    // Total Received Power
	float total_received_power; // Unit: dBm
};

struct Record {
	Record(RecordType record_type);
	Record(RecordType record_type, std::vector<glm::vec3> record_data);
	RecordType type;
	std::vector<glm::vec3> data;
};


#endif // !RECORD_H_