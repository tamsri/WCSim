#ifndef OBJECT_H
#define OBJECT_H

#include "glm/glm.hpp"

class Camera;
class Shader;

class Object {
public:
	virtual void Draw() const = 0;
	void DrawObject(Camera* camera) const;
	Shader* shader_;
	glm::vec3 position_, scale_, rotation_;
	glm::mat4 model_;
};
#endif	