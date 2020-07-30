#include "triangle.hpp"

#include <iostream>

#include "ray.hpp"


Triangle::Triangle(const std::vector<glm::vec3>& points, glm::vec3 normal)
{
	//id_ = global_id_++;
	std::cout << "id: " << id_;
	points_ = points;
	normal_ = normal;
	//std::cout << "Triangle making" << std::endl;
}

bool Triangle::IsHit(const Ray& ray, float & t) const
{
	const float k_epsilon = 0.000001f; /// ?? is it ok?
	// moller trumbore
	glm::vec3 v0v1 = points_[1] - points_[0];
	glm::vec3 v0v2 = points_[2] - points_[0];
	glm::vec3 p_vec = glm::cross(ray.GetDirection(), v0v2);
	float det = glm::dot(v0v1, p_vec);

	// det is close to 0, miss triangle
	if (det < k_epsilon && det >-k_epsilon) return false;

	// ray and triangle are parallel
	if (fabs(det) < k_epsilon) return false;

	float inverse_det = 1 / det;
	glm::vec3 t_vector = ray.GetOrigin() - points_[0];

	glm::vec3 q_vec = glm::cross(t_vector, v0v1);
	t = glm::dot(v0v2, q_vec) * inverse_det;
	return true;
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
