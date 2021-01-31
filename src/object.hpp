#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>

#include "transform.hpp"

class Camera;
class Shader;
class Transform;
class Object {
public:
	virtual void Draw() const = 0;
	void DrawObject(Camera* camera) const;
	void MoveTo(glm::vec3 position);
	void TransformTo(Transform transform);
	Shader * shader_;
	Transform transform_;
	glm::mat4 model_;
	glm::vec4 color_;
	static Shader * default_shader_;
	static Shader * ray_shader_;
};
#endif