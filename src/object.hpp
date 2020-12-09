#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>

#include "transform.hpp"

class Camera;
class Shader;

class Object {
public:
	virtual void Draw() const = 0;
	void DrawObject(Camera* camera) const;
	void MoveTo(glm::vec3 position);

	Shader * shader_;
	Transform transform_;
	glm::mat4 model_;
	glm::vec4 color_;
	static Shader * default_shader_;
	static Shader * ray_shader_;
};
#endif