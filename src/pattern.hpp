#ifndef PATTERN_H_
#define PATTERN_H_

#include <string>
#include <map>
#include <vector>

#include "object.hpp"

struct Vertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
};

struct Texture {
	unsigned int id;
	std::string type;
};

class Pattern : public Object{
	Pattern(std::map<float, std::map<float, float>> pattern_);

private:
	std::vector<glm::vec3> full_vertices_, normals_;
	std::vector<glm::vec2> uvs_;
	unsigned int vao_, vbo_;
};
#endif // !PATTERN_H_