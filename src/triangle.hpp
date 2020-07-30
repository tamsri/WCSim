#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "glm/glm.hpp"
#include <vector>
class Ray;


class Triangle {
public:
	Triangle(const std::vector<glm::vec3>& points, glm::vec3 normal);
	bool IsHit(const Ray & ray, float & t) const;
	glm::vec3 GetNormal()const;
	std::vector<glm::vec3> GetPoints() const;
	//static unsigned int global_id_;
private:
	std::vector<glm::vec3> points_;
	glm::vec3 normal_;
	unsigned int id_;
};
//unsigned int Triangle::global_id_ = 0;
#endif // !TRIANGLE_H