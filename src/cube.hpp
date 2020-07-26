#ifndef CUBE_H
#define CUBE_H

#include "glm/glm.hpp"

#include "object.hpp"

#include <glm/gtc/matrix_transform.hpp>

struct Transform;
class Camera;
class Shader;

class Cube : public Object {
public:
	Cube(Transform transform, Shader * shader);
	virtual void Draw() const;
private:
	unsigned int vao_, vbo_, ebo_;

};
#endif // !CUBE_H
