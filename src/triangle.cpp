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
	const float k_epsilon = 0.00000001f; /// ?? is it ok?

	glm::vec3 v0 = points_[0];
	glm::vec3 v1 = points_[1];
	glm::vec3 v2 = points_[2];
	glm::vec3 edge_1, edge_2, h, s, q;
	float a, f, u, v;
	edge_1 = v1 - v0;
	edge_2 = v2 - v0;
	h = glm::cross(ray.GetDirection(), edge_2);
	a = glm::dot(edge_1, h);

	//if (a > k_epsilon) return false; // able to get both side of the triangle
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

bool Triangle::IsHit(const Ray& ray, float& t, Triangle *& hit_triangle) const
{
	hit_triangle = &*(Triangle *)this;
	return IsHit(ray, t);
}

glm::vec3 Triangle::GetNormal() const
{
	return glm::vec3(normal_);
}

std::vector<glm::vec3> Triangle::GetPoints() const
{
	return points_;
}
