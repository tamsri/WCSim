#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "glm/glm.hpp"
#include <vector>
class Ray;


class Triangle {
public:
	Triangle(const std::vector<glm::vec3>& points);
	bool IsHit(const Ray & ray) const;
	glm::vec3 GetNormal()const;
	std::vector<glm::vec3> GetPoints() const;
private:
	std::vector<glm::vec3> points_;
	glm::vec3 normal_;
};
#endif // !TRIANGLE_H