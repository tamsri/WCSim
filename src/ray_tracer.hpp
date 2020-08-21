#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include <vector>
#include <map>
#include <utility>

#include <glm/glm.hpp>


class Shader;
class PolygonMesh;
class Ray;
class Triangle;
class Shader;
class Object;
class Camera;

struct FragmentRay {
	std::vector<glm::vec3> path_points; // ray and lenght
};

class FragmentVoxel {
public:
	FragmentVoxel(glm::vec3 position, PolygonMesh * map_);
	void ScanHit(PolygonMesh * map);

	glm::vec3 position_;
	std::map<Triangle *, bool> possible_hit_triangles;
	std::map<FragmentVoxel *, std::vector<FragmentRay>> rays_to_neighbours;
};

class RayTracer {
public:
	RayTracer(PolygonMesh * map, Shader * ray_shader);

	void Test();

	void InitializeVoxels(unsigned int width, unsigned int depth, unsigned int height);
	
	void Trace(FragmentVoxel & start_voxel, FragmentVoxel & end_voxel);
	bool IsDirectHit(glm::vec3 start_point, glm::vec3 end_point) const;
	bool IsOutdoor(glm::vec3 start_point) const;
	
	bool IsReflected(glm::vec3 start_point, glm::vec3 end_point, std::vector<FragmentRay*>& reflected_rays) const;
	glm::vec3 ReflectedPointOnTriangle(Triangle* triangle, glm::vec3 point) const;

	void DrawObjects(Camera * main_camera) const;

	unsigned int width_, depth_, height_;
	std::vector<Object*> objects_;
	Shader * ray_shader_;
	std::map<glm::vec3, FragmentVoxel *> voxels_; // may implement kb tree later
	PolygonMesh * map_;
};
#endif // !RAY_TRACER_H