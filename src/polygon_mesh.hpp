#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <iostream>
#include <set>
#include <utility>
#include <map>
#include <glm/glm.hpp>

#include "object.hpp"

class Triangle;
class Shader;
class Camera;
class KDTree;
class Ray;
struct Transform;
class RadiationPattern;

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
	PolygonMesh(const RadiationPattern& radiation_pattern);
	PolygonMesh(const std::string & path, Shader * shader, bool is_window_on);
	bool LoadObj(	const std::string& path);
	virtual void Draw() const;
	void UpdateTransform(Transform& transform);
	void SetupMesh();
	void GetBorders(float & min_x, float & max_x, float & min_z, float & max_z) const;
	bool IsHit(Ray & ray, float & t) const; // return the nearest hit distance
	bool IsHit(Ray& ray, float& t, Triangle *& hit_triangle) const; // return the nearest hit triangle
	bool IsHit(Ray& ray, std::set<std::pair<float, Triangle *>> & hit_triangles) const; // return the set of hit triangles

	std::vector<const Triangle*> GetObjects();

private:
	// For Visualisation
	std::vector<glm::vec3> full_vertices_, normals_;
	std::vector<glm::vec2> uvs_;
	
	// For Ray Tracer
	std::vector<const Triangle*> objects_;

	KDTree * tree_;
	unsigned int vao_, vbo_;

	float min_x_;
	float max_x_;
	float min_z_;
	float max_z_;
	std::vector<Vertex> vertices_;
};
#endif // !POLYGON_H