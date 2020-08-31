#ifndef RECEIVER_H
#define RECEIVER_H

#include <glm/glm.hpp>
#include "transform.hpp"

class Point;

class Receiver {

public:

	Receiver(Transform position);


	glm::vec3 position_;
	glm::vec3 velocity_;

	void Update();
	Point * point;

};
#endif //!RECEIVER_H