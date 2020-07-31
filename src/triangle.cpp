#include "triangle.hpp"

#include <iostream>

#include "ray.hpp"


Triangle::Triangle(const std::vector<glm::vec3>& points, glm::vec3 normal)
{
	//id_ = global_id_++;
	//std::cout << "id: " << id_;
	points_ = points;
	normal_ = normal;
	//std::cout << "Triangle making" << std::endl;
}

bool Triangle::IsHit(const Ray& ray, float & t) const
{
	const float k_epsilon = 0.000001f; /// ?? is it ok?
	// moller trumbore
	//glm::vec3 p0p1 = points_[1] - points_[0];
	//glm::vec3 p0p2 = points_[2] - points_[0];
	//glm::vec3 p_vec = glm::cross(ray.GetDirection(), p0p2);
	//float det = glm::dot(p0p1, p_vec);

	//// det is close to 0, miss triangle
	////if (det < k_epsilon) return false;

	//// ray and triangle are parallel
	//if (fabs(det) < k_epsilon) return false;

	//float inverse_det = 1 / det;
	//glm::vec3 t_vector = ray.GetOrigin() - points_[0];

	//glm::vec3 q_vec = glm::cross(t_vector, p0p1);
	//t = glm::dot(p0p2, q_vec) * inverse_det;
	//
	//return true;

	// second try
	glm::vec3 v0 = points_[0];
	glm::vec3 v1 = points_[1];
	glm::vec3 v2 = points_[2];
	glm::vec3 edge_1, edge_2, h, s, q;
	float a, f, u, v;
	edge_1 = v1 - v0;
	edge_2 = v2 - v0;
	h = glm::cross(ray.GetDirection(), edge_2);
	a = glm::dot(edge_1, h);

	if (a > fabs(k_epsilon)) return false;
	f = 1.0f / a;
	s = ray.GetOrigin() - v0;
	u = f * glm::dot(s, h);
	if (u < 0.0f || u > 1.0f) return false;
	q = glm::cross(s, edge_1);
	v = f * glm::dot(ray.GetDirection(), q);
	if (v < 0.0f || u + v > 1.0f) return false;

	t = f * glm::dot(edge_2, q);
	if (t > k_epsilon) return true;

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
