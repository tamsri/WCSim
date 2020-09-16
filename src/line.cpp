#include "line.hpp"

#include "glad/glad.h"
#include "glfw/glfw3.h"

#include "glm/gtc/matrix_transform.hpp"


Line::Line(glm::vec3 start_position, glm::vec3 end_position)
{
	color_ = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	shader_ = ray_shader_;
	model_ = glm::mat4(1.0f);
	float vertices[] = {
		start_position.x, start_position.y, start_position.z,
		end_position.x, end_position.y, end_position.z,

	};
	glGenVertexArrays(1, &vao_);
	glGenBuffers(1, &vbo_);

	glBindVertexArray(vao_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// safe end
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void Line::SetColor(glm::vec4 color) {
	color_ = color;
}
void Line::Draw() const
{
	glBindVertexArray(vao_);
	glDrawArrays(GL_LINES, 0, 2);
}
