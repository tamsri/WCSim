#include "ray.hpp"

#include <iostream>
#include <string>

#include "glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"

#include "transform.hpp"


Ray::Ray(glm::vec3 origin, glm::vec3 direction, Shader * shader)
{
	// Ray - Tracer
	origin_ = origin;
	direction_ = direction;
	// Visualizer
	transform_.position = glm::vec3(0.0f);
	transform_.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	transform_.rotation = glm::vec3(0.0f);
	shader_ = shader;

	model_ = glm::mat4(1.0f);
}

void Ray::SetRayColor(glm::vec4 color)
{
	color_ = color;
}


Ray::Ray(glm::vec3 origin, glm::vec3 direction) {
	// Ray - Tracer
	origin_ = origin;
	direction_ = direction;
	// Visualizer
	transform_.position = glm::vec3(0.0f);
	transform_.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	transform_.rotation = glm::vec3(0.0f);
	shader_ = ray_shader_;

	color_ = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	model_ = glm::mat4(1.0f);
}


glm::vec3 Ray::GetOrigin() const
{
	return glm::vec3(origin_);
}

glm::vec3 Ray::GetDirection() const
{
	return glm::vec3(direction_);
}

glm::vec3 Ray::PointAtLength(float length) const
{
	return glm::vec3(origin_+direction_*length);
}

void Ray::InitializeRay(float length)
{
	glm::vec3 destination = PointAtLength(length);

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
