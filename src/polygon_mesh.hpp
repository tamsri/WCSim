#ifndef POLYGON_H
#define POLYGON_H

#include<vector>
#include<iostream>

#include <glm/glm.hpp>

#include "triangle.hpp"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 texture_coord;
};

struct Texture{
	unsigned int id;
	std::string type;
};

class PolygonMesh {

public:
	PolygonMesh(const std::string & path);

	void SetupMesh();
private:
	std::vector<glm::vec3> vertices_, normal_;
	std::vector<std::vector<int>> faces_;
	std::vector<const Triangle*> objects;

	unsigned int vao_, vbo_;
};
#endif // !POLYGON_H