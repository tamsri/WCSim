#include "triangle.hpp"

#include "ray.hpp"

Triangle::Triangle(const std::vector<glm::vec3>& points)
{
	points_ = points;
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
