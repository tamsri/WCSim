#ifndef RAY_H
#define RAY_H

#include "glm/glm.hpp"


class Ray {
public:
	Ray(glm::vec3 origin, glm::vec3 direction);
	glm::vec3 GetOrigin() const;
	glm::vec3 GetDirection() const;
private:
	glm::vec3 origin_;
	glm::vec3 direction_;
};
#endif // !RAY_H