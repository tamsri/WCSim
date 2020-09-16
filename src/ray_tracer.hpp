#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include <vector>
#include <utility>
#include <cstdlib>


#include <glm/glm.hpp>

#include "record.hpp"

class Shader;
class PolygonMesh;
class Ray;
class Triangle;
class Shader;
class Object;
class Camera;
class Transmitter;
class Receiver;

struct Record;
struct Point;
struct Result;

class RayTracer {
public:
	RayTracer(PolygonMesh * map);

	void Test();
	
	// Ray Tracing Part
	void Trace(Point * start_point, Point * end_point, std::vector<Record> & records);
	void GetDrawComponents(Point * start_point, Point * end_point, std::vector<Record> & records, std::vector<Object *> & objects) const;
	bool CalculatePathLoss(Transmitter * transmitter, Receiver * receiver, const std::vector<Record> & records ,Result & result) const;
	// Line of Sight
	bool IsDirectHit(glm::vec3 start_point, glm::vec3 end_point) const;
	//bool IsOutdoor(glm::vec3 start_point) const;
	
	// Reflection
	void ScanHit(Point * position) const;
	bool IsReflected(Point * start_point, Point * end_point, std::vector<glm::vec3> & reflected_points) const;
	float CalculateReflectionCofficient(glm::vec3 start_position, glm::vec3 end_position, glm::vec3 reflection_position) const;
	glm::vec3 ReflectedPointOnTriangle(const Triangle * triangle, glm::vec3 point) const ;

	// Diffraction
	bool IsKnifeEdgeDiffraction(Point * start_point, Point * end_point, std::vector<glm::vec3> & edges_points) const;
	bool FindEdge(glm::vec3 start_position, glm::vec3 end_position, glm::vec3 & edge_position) const;
	glm::vec3 NearestEdgeFromPoint(glm::vec3 point_position, std::vector<glm::vec3> & edges_points ) const;
	void CleanEdgePoints(std::vector<glm::vec3> & edges_points) const;
	float CalculateSingleKnifeEdge(glm::vec3 start_position, glm::vec3 edge_position, glm::vec3 end_position, float frequency) const;
	float CalculateDiffractionByV(float v) const;
	float CalculateVOfEdge(glm::vec3 start_position, glm::vec3 edge_position, glm::vec3 end_position, float frequency) const;
	void CalculateCorrectionCosines(glm::vec3 start_position, std::vector<glm::vec3> edges, glm::vec3 end_position, std::pair<float, float> & calculated_cosines) const;

	PolygonMesh * map_;

};
#endif // !RAY_TRACER_H