#include "ray.hpp"

#include <iostream>

#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"

#include "shader.hpp"

Ray::Ray(glm::vec3 origin, glm::vec3 direction, Shader * shader)
{
	// Ray - Tracer
	origin_ = origin;
	direction_ = direction;
	// Visualize
	position_ = glm::vec3(0.0f, 0.0f, 0.0f);
	scale_ = glm::vec3(1.0f, 1.0f, 1.0f);
	rotation_ = glm::vec3(0.0f);
	shader_ = shader;

	model_ = glm::translate(glm::mat4(1.0f), position_);
	model_ = glm::scale(model_, scale_);

}

glm::vec3 Ray::GetOrigin() const
{
	return glm::vec3(origin_);
}

glm::vec3 Ray::GetDirection() const
{
	return glm::vec3(direction_);
}

void Ray::InitializeRay(float length)
{
	glm::vec3 destination = origin_ + length * direction_;
	std::cout << destination.x << " ";
	std::cout << destination.y << " ";
	std::cout << destination.z << std::endl;
	float line[] = {
		origin_.x, origin_.y, origin_.z,
		destination.x, destination.y, destination.z
	}; 
	glGenVertexArrays(1, &vao_);
	glGenBuffers(1, &vbo_);

	glBindVertexArray(vao_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), line, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// safe end
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Ray::Draw() const
{
	glBindVertexArray(vao_);
	glDrawArrays(GL_LINES, 0, 2);
}
