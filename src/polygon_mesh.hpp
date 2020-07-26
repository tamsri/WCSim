#ifndef POLYGON_H
#define POLYGON_H

#include<vector>
#include<iostream>

#include <glm/glm.hpp>

#include "object.hpp"

class Triangle;
class Shader;
class Camera;
class KDTree;
class Ray;


struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 texture_coord;
};

struct Texture{
	unsigned int id;
	std::string type;
};

class PolygonMesh : public Object {

public:
	PolygonMesh(const std::string & path, Shader * shader);
	bool LoadObj(	const std::string& path, 
					std::vector <glm::vec3> & vertices,
					std::vector <glm::vec3> & uvs,
					std::vector <glm::vec3> & normals);
	virtual void Draw() const;
	void SetupMesh();
	bool IsHit(Ray& ray) const;
private:
	// for ray tracer
	std::vector<glm::vec3> vertices_, normals_;
	std::vector<std::vector<int>> faces_;
	std::vector<const Triangle*> objects_;

	KDTree * tree_;
	unsigned int vao_, vbo_;
};
#endif // !POLYGON_H