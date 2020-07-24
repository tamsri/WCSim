#ifndef CUBE_H
#define CUBE_H

#include "glm/glm.hpp"

#include "object.hpp"

#include <glm/gtc/matrix_transform.hpp>

class Camera;
class Shader;

class Cube : public Object {
public:
	Cube(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, Shader * shader);
	virtual void Draw() const;
private:
	unsigned int vao_, vbo_, ebo_;

};
#endif // !CUBE_H
