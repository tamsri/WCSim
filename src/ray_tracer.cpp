#include "ray_tracer.hpp"

#include <iostream>

#include "object.hpp"
#include "ray.hpp"
#include "triangle.hpp"
#include "polygon_mesh.hpp"
#include "shader.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
RayTracer::RayTracer(PolygonMesh * map,  Shader * ray_shader):map_(map), ray_shader_(ray_shader)
{
    // Initialize voxel spaces; ?
    Test();
    InitializeVoxels(100, 100, 5);
}

void RayTracer::Test()
{

    // LOS test
    
    // First test [+] works
    glm::vec3 start_point = glm::vec3(10.00f, 10.00f, 2.0f);
    glm::vec3 end_point = glm::vec3(-10.00f, 5.00f, 10.0f);
    if (IsDirectHit(start_point, end_point)) {
        std::cout << "Yes, it's LOS" << std::endl;
    }
    else {
        std::cout << "No, it's NLOS" << std::endl;
    }

    //

}

void RayTracer::InitializeVoxels(unsigned int width, unsigned int depth, unsigned int height) 
{
    width_ = width; depth_ = depth; height_ = height;
    std::cout << "Initializing voxels." << std::endl;
    float voxels_per_m_sq = 1.0f;
    float increase = 1.0f / (float)voxels_per_m_sq;
    for(float x = -(width/2.0f); x < width/2.0f; x += increase)
        for(float y = -(depth/2.0f); y < depth/2.0f; y += increase)
            for (float z = -(height/2.0f); z < height/2.0f; z+= increase) {
                glm::vec3 position = glm::vec3{ x,y,z };
                std::cout << position.x << "," << position.y << "," << position.z << std::endl;
                FragmentVoxel * voxel = new FragmentVoxel(position);
                voxels_.insert(std::pair<glm::vec3, FragmentVoxel *>( position, voxel ) );
            }
    std::cout << "Initializing voxels completed." << std::endl;

    return;
}

void RayTracer::ScanHit()
{
    
}


void RayTracer::Trace(FragmentVoxel & start_voxel, FragmentVoxel & end_voxel)
{
    glm::vec3 start_point = start_voxel.position_;
    glm::vec3 end_point = end_voxel.position_;
    // find LOS
    if (IsDirectHit(start_point, end_point)) start_voxel.rays_to_neighbours[&end_voxel].push_back(FragmentRay{ std::vector<glm::vec3>{start_point, end_point} });
    // find NLOS
}

bool RayTracer::IsDirectHit(glm::vec3 start_point, glm::vec3 end_point) const
{
    // get direction from start point to end point
    glm::vec3 origin = start_point;
    glm::vec3 direction = glm::normalize(end_point - origin);
    float start_to_end_distance = glm::distance(start_point, end_point);
    Ray ray{ origin, direction, ray_shader_ };
    float distance = -1;
    // trace the ray on this direction 
    // check if the direction hit something and the t is not betwen start_point to end_point length
    if (map_->IsHit(ray, distance) && distance != -1 && distance < start_to_end_distance) {
        std::cout << "hit something at distance: " << distance << std::endl;
        return false;
    }
    return true;
}

bool RayTracer::IsReflected(glm::vec3 start_point, glm::vec3 end_point, std::vector<FragmentRay *> & reflected_rays) const
{
    // search the hitable triangles between two points
    glm::vec3 origin = start_point;
    // match the co-exist triangles between two points

    // reflect one of the point on the triangle plane

    // trace from the reflected point 
    return false;
}

bool RayTracer::IsOutdoor(glm::vec3 start_point) const
{
    return false;
}

glm::vec3 RayTracer::ReflectedPointOnTriangle(Triangle * triangle, glm::vec3 points) const
{
    /// algorithms
  
    // 1. construct the plane from 3 points (non-collinear points)
    glm::vec3 p_0 = triangle->GetPoints()[0];
    glm::vec3 p_1 = triangle->GetPoints()[1];
    glm::vec3 p_2 = triangle->GetPoints()[2];
    glm::vec3 p_01 = p_1 - p_0;
    glm::vec3 p_02 = p_2 - p_1;

    glm::vec3 n = glm::cross(p_01, p_02);
    float b = n.x * p_0.x + n.y * p_0.y + n.z * p_0.z;
    // 2. mirror the point from the plane (https://youtu.be/fvXG-DVx6z0)
    float t = (b - points.x - points.y - points.z) / (n.x + n.y + n.z); //distance from point to plane

    glm::vec3 m = glm::vec3 (points.x + t * n.x, points.y + t * n.y, points.z + t * n.z); // points on mirror


    return  m * 2.0f - points; // reverse average point from average point
}

void RayTracer::DrawObjects(Camera * main_camera) const
{
    for (auto & object : objects_) {
        object->DrawObject(main_camera);
    }
}

FragmentVoxel::FragmentVoxel(glm::vec3 position) :position_(position)
{
}
