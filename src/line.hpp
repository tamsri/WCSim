#ifndef LINE_H
#define LINE_H
#include "object.hpp"

class Line : Object {

public:
	Line(glm::vec3 start_position, glm::vec3 end_position);

	void SetColor(glm::vec4 color);
	virtual void Draw() const;

private:
	unsigned int vao_, vbo_;
};
#endif // !LINE_H
