#include "ray.hpp"

Ray::Ray(glm::vec3 origin, glm::vec3 direction)
{
	origin_ = origin;
	direction_ = direction;
}

glm::vec3 Ray::GetOrigin() const
{
	return glm::vec3(origin_);
}

glm::vec3 Ray::GetDirection() const
{
	return glm::vec3(direction_);
}
