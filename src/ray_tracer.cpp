#include "ray_tracer.hpp"

#include <iostream>
#include <set>
#include <utility>

#include "object.hpp"
#include "cube.hpp"
#include "ray.hpp"
#include "triangle.hpp"
#include "polygon_mesh.hpp"
#include "shader.hpp"

#include "transform.hpp"


RayTracer::RayTracer(PolygonMesh * map,  Shader * ray_shader):map_(map), ray_shader_(ray_shader)
{
    // Initialize voxel spaces; ?
    Test();
    //InitializeVoxels(100, 100, 5);
}

void RayTracer::Test()
{

    // LOS test
    
    // First test [+] works
    glm::vec3 start_point = glm::vec3(10.00f, 15.00f, -12.0f);
    glm::vec3 end_point = glm::vec3(-35.00f, 10.00F, 60.0f);
    if (IsDirectHit(start_point, end_point)) {
        std::cout << "Yes, it's LOS" << std::endl;
        Ray* ray = new Ray(start_point, glm::normalize(end_point - start_point), ray_shader_);
        ray->InitializeRay(glm::distance(end_point, start_point));
        objects_.push_back(ray);
    }
    else {
        std::cout << "No, it's NLOS" << std::endl;
    }

    // REFLECTION
    FragmentVoxel first_voxel{ start_point , map_};
    FragmentVoxel second_voxel{ end_point , map_};

    std::vector<Triangle*> matched_triangles;
    std::map<Triangle*, bool> checker;
    for (auto const& [key, value] : first_voxel.possible_hit_triangles) {
        if (second_voxel.possible_hit_triangles[key]) matched_triangles.push_back(key);
    };

    /*std::cout << "first hit triangles: " << first_voxel.possible_hit_triangles.size() << std::endl;
    std::cout << "first hit triangles: " << second_voxel.possible_hit_triangles.size() << std::endl;
    std::cout << "matched triangles : " << matched_triangles.size() << std::endl;*/
    
    glm::vec3 check_position = first_voxel.position_;
    std::cout << "testing reflection" << std::endl;
    std::cout << "check position: " << check_position.x << ", " << check_position.y << ", " << check_position.z << std::endl;
    Cube* start_cube = new Cube(Transform{ start_point, glm::vec3(2.0f, .5f, .5f), glm::vec3(0.0f) }, ray_shader_);
    Cube* end_cube = new Cube(Transform{ end_point, glm::vec3(2.0f, .5f, .5f), glm::vec3(0.0f) }, ray_shader_);
    objects_.push_back(start_cube);
    objects_.push_back(end_cube);



    for (unsigned int i = 0; i < matched_triangles.size(); ++i) {
        /*std::cout << "triangle>> p1:";
        std::cout << matched_triangles[i]->GetPoints()[0].x << "," << matched_triangles[i]->GetPoints()[0].y << "," << matched_triangles[i]->GetPoints()[0].z << " //p2: ";
        std::cout << matched_triangles[i]->GetPoints()[1].x << "," << matched_triangles[i]->GetPoints()[1].y << "," << matched_triangles[i]->GetPoints()[1].z << " //p3: ";
        std::cout << matched_triangles[i]->GetPoints()[2].x << "," << matched_triangles[i]->GetPoints()[2].y << "," << matched_triangles[i]->GetPoints()[2].z << std::endl;*/
        Triangle * check_triangle = matched_triangles[i];
        glm::vec3 reflect_position = ReflectedPointOnTriangle( check_triangle, check_position);

        
        Cube* reflect_cube = new Cube(Transform{ reflect_position, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f) }, ray_shader_);
        objects_.push_back(reflect_cube);
        std::cout << "reflect position: " << reflect_position.x << ", " << reflect_position.y << ", " << reflect_position.z << std::endl;/**/
        
        /* reflected points */
        glm::vec3 direction_to = glm::normalize(end_point - reflect_position);
        float distance_reflect_to_end = glm::distance(reflect_position, end_point);
        Ray * ray = new Ray( reflect_position, direction_to, ray_shader_ );
        ray->InitializeRay(distance_reflect_to_end);
        //objects_.push_back(ray);
        float temp_t;
        std::set<std::pair<float,Triangle*>> hit_triangles;
        if (map_->IsHit(*ray, hit_triangles)) {
            // hit at the back because we don't care if 
            /*if (std::get<1>(*hit_triangles.end()) == check_triangle)
            {
                std::cout << "It's reflected!!!" << std::endl;
            }
            else std::cout << "It hit reflected on the object but got hit something else " << std::endl;*/
            bool is_check_triangle_hit_by_reflect_point = false;
            float distance_to_check_triangle_from_reflect = 0;
            float is_hit_something_else = false;
            for (auto& [distance, triangle] : hit_triangles) {
                if (triangle == check_triangle) {
                    distance_to_check_triangle_from_reflect = distance;
                    is_check_triangle_hit_by_reflect_point = true;
                    break;
                }
            }
            if (is_check_triangle_hit_by_reflect_point)
            {
                for (auto& [distance, triangle] : hit_triangles) {
                    if (distance > distance_to_check_triangle_from_reflect
                        && distance < distance_reflect_to_end) {
                        is_hit_something_else = true;
                        std::cout << "Hit at distance: " << distance << std::endl;

                    }
                }
                if (is_hit_something_else) {
                    std::cout << "it hit something else, sorry. " << std::endl;
                }
                else {
                    // it reflected
                    std::cout << "it reflects" << std::endl;
                    glm::vec3 hit_position = reflect_position + direction_to * distance_to_check_triangle_from_reflect;

                    Cube * reflect_cube = new Cube(Transform{ hit_position, glm::vec3(.3f, .3f, .3f), glm::vec3(0.0f) }, ray_shader_);
                    objects_.push_back(reflect_cube);

                    Ray* from_reflect = new Ray(hit_position, direction_to, ray_shader_);
                    from_reflect->InitializeRay(glm::distance(hit_position, end_point));
                    objects_.push_back(from_reflect);

                    Ray* to_triangle = new Ray(start_point, glm::normalize(hit_position - start_point), ray_shader_);
                    to_triangle->InitializeRay(glm::distance(hit_position, start_point));
                    objects_.push_back(to_triangle);

                    // store only hit point!!
                }
            }
            else {
                std::cout << "N0!! it does't reflect anything" << std::endl;
            }
        }
        else {
            std::cout << "This ray doesn't hit anything" << std::endl;
        }

    }

}

void RayTracer::InitializeVoxels(unsigned int width, unsigned int depth, unsigned int height) 
{
    // TODO: implement voxels initialization if possible
    /*width_ = width; depth_ = depth; height_ = height;
    std::cout << "Initializing voxels." << std::endl;
    float voxels_per_m_sq = 10.0f;
    float increase = 1.0f / (float)voxels_per_m_sq;
    for(float x = -(width/2.0f); x < width/2.0f; x += increase)
        for(float y = -(depth/2.0f); y < depth/2.0f; y += increase)
            for (float z = -(height/2.0f); z < height/2.0f; z+= increase) {
                glm::vec3 position = glm::vec3{ x,y,z };
                //std::cout << position.x << "," << position.y << "," << position.z << std::endl;
                FragmentVoxel * voxel = new FragmentVoxel(position, map_);
                voxels_.push_back(voxel);
            }
    std::cout << "Initializing voxels completed." << std::endl;*/

    return;
}

void FragmentVoxel::ScanHit(PolygonMesh * map)
{
    glm::vec3 position = position_;
    glm::vec4 direction = { 1.0f , 0.0f, 0.0f, 1.0f };
    float scan_precision = 0.5;
    for (float i = 0; i < 360; i += scan_precision)
        for (float j = 0; j < 360; j += scan_precision) {
            auto trans_direction = glm::rotate(glm::mat4(1.0f), glm::radians(i), glm::vec3(0.0f, 1.0f, 0.0f));
            trans_direction = glm::rotate(trans_direction, glm::radians(j), glm::vec3(0.0f, 0.0f, 1.0f));
            auto new_direction = trans_direction * direction;
            glm::vec3 i_direction = glm::vec3{ new_direction.x, new_direction.y, new_direction.z };
            Ray * ray = new Ray ( position, i_direction );
            Triangle * hit_triangle = nullptr;
            float hit_distance; // doesnt do anything // maybe implement faster function. 
            if (map->IsHit( * ray, hit_distance, hit_triangle)) {
                possible_hit_triangles[hit_triangle] = true;
            }
            else {
                delete ray;
            }
        }
}


void RayTracer::Trace(FragmentVoxel & start_voxel, FragmentVoxel & end_voxel)
{
    glm::vec3 start_point = start_voxel.position_;
    glm::vec3 end_point = end_voxel.position_;
    // find if LOS
    if (IsDirectHit(start_point, end_point)) start_voxel.rays_to_neighbours[&end_voxel].push_back(FragmentRay{ std::vector<glm::vec3>{start_point, end_point} });
    // if NLOS, find edge
    
    // find possible reflections

    
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
        //std::cout << "hit something at distance: " << distance << std::endl;
        return false;
    }
    return true;
}

bool RayTracer::IsReflected(glm::vec3 start_point, glm::vec3 end_point, std::vector<FragmentRay *> & reflected_rays) const
{
    // search the hitable triangles between two points
    glm::vec3 origin = start_point;
    glm::vec3 destination = end_point;
    // match the co-exist triangles between two points
    
    /// TODO get matches triangles

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
    /*Cube* check_tr_1 = new Cube(Transform{ glm::vec3{p_0.x,p_0.y,p_0.z}, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f) }, ray_shader_);
    Cube* check_tr_2 = new Cube(Transform{ glm::vec3{p_1.x,p_1.y,p_1.z}, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f) }, ray_shader_);
    Cube* check_tr_3 = new Cube(Transform{ glm::vec3{p_2.x,p_2.y,p_2.z}, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f) }, ray_shader_);
    objects_.push_back(check_tr_1);
    objects_.push_back(check_tr_2);
    objects_.push_back(check_tr_3);*/ // display the corner of hit triangles.

    //glm::vec3 p_01 = glm::normalize(p_1 - p_0);
    //glm::vec3 p_02 = glm::normalize(p_2 - p_0);
    //std::cout << "points" << std::endl;
    //std::cout << "p01: " << p_01.x << ", " << p_01.y << ", " << p_01.z << std::endl;
    //std::cout << "p02: " << p_02.x << ", " << p_02.y << ", " << p_02.z << std::endl;

    //glm::vec3 n_c = glm::normalize(glm::cross(p_01, p_02));
    //std::cout << "calcualted normal " << n_c.x << ", " << n_c.y << ", " << n_c.z << std::endl;
    glm::vec3 n = triangle->GetNormal();
    //std::cout << "stored normal: " << n.x << ", " << n.y << ", " << n.z << std::endl; /// todo: implemenet to not calcualte the normal as the obj already did it
    //n = n_c;

    float b = (n.x * p_1.x) + (n.y * p_1.y) + (n.z * p_1.z);
    //std::cout << "plane: " << n.x << "x + " << n.y << "y + " << n.z << "z = " << b << std::endl;
    // 2. mirror the point from the plane
    float t = (b - (points.x*n.x + points.y*n.y + points.z*n.z )) / (n.x*n.x + n.y*n.y + n.z*n.z); //distance from point to plane

    /// DEBUG mirror point
    glm::vec3 m = glm::vec3 (points.x + t * n.x, points.y + t * n.y, points.z + t * n.z); // points on mirror
    //Cube * reflect_cube = new Cube(Transform{ m, glm::vec3(.3f, .3f, .3f), glm::vec3(0.0f) }, ray_shader_);
    //objects_.push_back(reflect_cube);

    return  points + 2*t*n; // reverse average point from average point 
}

void RayTracer::DrawObjects(Camera * main_camera) const
{
    for (auto & object : objects_) {
        object->DrawObject(main_camera);
    }
}

FragmentVoxel::FragmentVoxel(glm::vec3 position, PolygonMesh * map_):position_(position)
{
    ScanHit(map_);
}