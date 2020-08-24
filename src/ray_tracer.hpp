#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include <vector>
#include <utility>



#include <glm/glm.hpp>

#include "record.hpp"

class Shader;
class PolygonMesh;
class Ray;
class Triangle;
class Shader;
class Object;
class Camera;
struct Record;
struct Point;
 

class RayTracer {
public:
	RayTracer(PolygonMesh * map);

	void Test();

	void InitializeVoxels(unsigned int width, unsigned int depth, unsigned int height);
	
	
	// Ray Tracing Part
	void Trace(glm::vec3 start_position, glm::vec3 end_position);
	
	// Line of Sight
	bool IsDirectHit(glm::vec3 start_point, glm::vec3 end_point) const;
	bool IsOutdoor(glm::vec3 start_point) const;
	
	// Reflection
	void ScanHit(glm::vec3 position);
	bool IsReflected(glm::vec3 start_point, glm::vec3 end_point, std::vector<glm::vec3> & reflected_points) const;
	glm::vec3 ReflectedPointOnTriangle(Triangle * triangle, glm::vec3 point) const;

	// Diffraction
	bool IsKnifeEdgeDiffraction(glm::vec3 start_point, glm::vec3 end_point, std::vector<glm::vec3> & edges_points);



	void DrawObjects(Camera * main_camera) const;

	unsigned int width_, depth_, height_;
	std::vector<Object*> objects_;



	point_map points_; // may implement kb tree later
	std::vector<Record*> records_; // all records being made (delete later)
	PolygonMesh * map_;
};
#endif // !RAY_TRACER_H