#include "triangle.hpp"

#include <iostream>

#include "ray.hpp"


Triangle::Triangle(const std::vector<glm::vec3>& points, glm::vec3 normal)
{
	points_ = points;
	normal_ = normal;
	//std::cout << "Triangle making" << std::endl;
}

bool Triangle::IsHit(const Ray& ray) const
{
	//glm::vec3 vector_p = glm::determinant(ray.GetDirection(), points_[] - points_[] );
	return false;
}

glm::vec3 Triangle::GetNormal() const
{
	return glm::vec3(normal_);
}

std::vector<glm::vec3> Triangle::GetPoints() const
{
	/// check if this will waste memory
	return points_;
}
