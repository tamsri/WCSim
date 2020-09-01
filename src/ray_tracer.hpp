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
struct Record;
struct Point;
 

class RayTracer {
public:
	RayTracer(PolygonMesh * map);

	void Test();

	void InitializeVoxels(unsigned int width, unsigned int depth, unsigned int height);
	
	
	// Ray Tracing Part
	void Trace(Point * start_point, Point * end_point);
	void InitializeDrawPointsComponents(Point * start_point, Point * end_point);
	bool CalculatePathLoss(Point* transmitter_point, Point* receiver_point, float & attenuation_in_dB, float frequency_in_GHz);
	// Line of Sight
	bool IsDirectHit(glm::vec3 start_point, glm::vec3 end_point) const;
	bool IsOutdoor(glm::vec3 start_point) const;
	
	// Reflection
	void ScanHit(Point * position);
	bool IsReflected(Point * start_point, Point * end_point, std::vector<glm::vec3> & reflected_points);
	
	glm::vec3 ReflectedPointOnTriangle(Triangle * triangle, glm::vec3 point) ;

	// Diffraction
	bool IsKnifeEdgeDiffraction(Point * start_point, Point * end_point, std::vector<glm::vec3> & edges_points);
	bool FindEdge(glm::vec3 start_position, glm::vec3 end_position, glm::vec3 & edge_position);
	glm::vec3 NearestEdgeFromPoint(glm::vec3 point_position, std::vector<glm::vec3> & edges_points );
	void CleanEdgePoints(std::vector<glm::vec3> & edges_points);
	float CalculateSingleKnifeEdge(glm::vec3 start_position, glm::vec3 edge_position, glm::vec3 end_position, float frequency);
	float CalculateDiffractionByV(float v);
	float CalculateVOfEdge(glm::vec3 start_position, glm::vec3 edge_position, glm::vec3 end_position, float frequency);
	void CalculateCorrectionCosines(glm::vec3 start_position, std::vector<glm::vec3> edges, glm::vec3 end_position, std::pair<float, float> & calculated_cosines);

	void DrawObjects(Camera * main_camera) const;

	unsigned int width_, depth_, height_;
	std::vector<Object*> objects_;



	point_map points_; // map of the points dict of <glm::vec3, Point*>
	Point * InitializeOrCallPoint(glm::vec3 initialized_point); // initialize the point and scan

	std::vector<Record*> records_; // all records being made (delete later)
	Record * direct_record_; // direct pointer for LOS (save space)
	PolygonMesh * map_;

};
#endif // !RAY_TRACER_H