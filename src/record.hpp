#ifndef RECORD_H_
#define RECORD_H_

#include <vector>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include <unordered_map>
#include <map>

class Triangle;

enum RecordType : int {
	kDirect = 0,
	kReflect,
	kEdgeDiffraction
};

struct Result {
	float direct_path_loss_in_linear;
	float reflection_loss_in_linear;
	float diffraction_loss_in_linear;

	float direct_path_tx_gain;
	std::vector<std::pair<glm::vec3, float>> reflection_loss_gain;
	float diffraction_loss_gain;
	
	float total_loss;
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

	std::unordered_map <Point*, std::vector<const Record *>> neighbour_record;

};

typedef std::unordered_map<glm::vec3, Point*> point_map;

#endif // !RECORD_H_