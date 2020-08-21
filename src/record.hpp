#ifndef RECORD_H_
#define RECORD_H_

#include <vector>
#include <map>

#include <glm/glm.hpp>

class Triangle;

enum RecordType : int {
	kDirect = 0,
	kReflect,
	kEdgeDiffraction
};

struct Record {
	RecordType type;
	std::vector<glm::vec3> record_data;
};

struct Point {
	glm::vec3 position;

	std::vector <Triangle*> hit_triangles;
	std::map <Point*, std::vector<Record* >> neighbor_record;
};

#endif // !RECORD_H_