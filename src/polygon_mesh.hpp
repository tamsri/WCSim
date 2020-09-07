#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <iostream>
#include <set>
#include <utility>

#include <glm/glm.hpp>

#include "object.hpp"

class Triangle;
class Shader;
class Camera;
class KDTree;
class Ray;


struct Vertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
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
	bool IsHit(Ray & ray, float & t) const; // return the nearest hit distance
	bool IsHit(Ray& ray, float& t, Triangle *& hit_triangle) const; // return the nearest hit triangle
	bool IsHit(Ray& ray, std::set<std::pair<float, Triangle *>> & hit_triangles) const; // return the set of hit triangles

	std::vector<const Triangle*> GetObjects();

private:
	// for ray tracer
	std::vector<glm::vec3> full_vertices_, normals_;
	std::vector<glm::vec2> uvs_;
	
	std::vector<const Triangle*> objects_;


	KDTree * tree_;
	unsigned int vao_, vbo_;

	std::vector<Vertex> vertices_;
};
#endif // !POLYGON_H