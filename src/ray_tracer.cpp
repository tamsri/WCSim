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

#include <chrono>

#include "transform.hpp"


RayTracer::RayTracer(PolygonMesh * map):map_(map)
{
    // Initialize direct record
    direct_record_ = new Record(RecordType::kDirect);
    records_.push_back(direct_record_);

    // Initialize voxel spaces; ?
    Test();
    //InitializeVoxels(100, 100, 5);
}

void RayTracer::Test()
{

    // LOS test
    // First test [+] works
    glm::vec3 start_position = glm::vec3(-15.00f, 5.00f, 40.0f);
    glm::vec3 end_position = glm::vec3(20.0f, 3.00F, 6.0f);
    glm::vec3 end_position_2 = glm::vec3(10.0f, 10.0f, 20.0f);
    Point* start_point = InitializeOrCallPoint(start_position);
    Point* end_point = InitializeOrCallPoint(end_position);
    Point* end_point_2 = InitializeOrCallPoint(end_position_2);
    // Test the trace function
    //Trace(start_point, end_point);
    std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
    Trace(start_point, end_point);
    std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
    std::cout << "Ray Tracer took " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count()/1e3 << "ms.\n";
    /*
    if (IsDirectHit(start_point, end_point)) {
        std::cout << "Yes, it's LOS" << std::endl;
        Ray* ray = new Ray(start_point, glm::normalize(end_point - start_point));
        ray->InitializeRay(glm::distance(end_point, start_point));
        objects_.push_back(ray);
    }
    else {
        std::cout << "No, it's NLOS" << std::endl;
        // find edge
    
    }*/

    //
    //std::vector<Triangle*> matched_triangles;
    //std::map<Triangle*, bool> checker;
    //for (auto const& [key, value] : first_voxel.possible_hit_triangles) {
    //    if (second_voxel.possible_hit_triangles[key]) matched_triangles.push_back(key);
    //};

    ///*std::cout << "first hit triangles: " << first_voxel.possible_hit_triangles.size() << std::endl;
    //std::cout << "first hit triangles: " << second_voxel.possible_hit_triangles.size() << std::endl;
    //std::cout << "matched triangles : " << matched_triangles.size() << std::endl;*/
    //
    //glm::vec3 check_position = first_voxel.position_;
    //std::cout << "testing reflection" << std::endl;
    //std::cout << "check position: " << check_position.x << ", " << check_position.y << ", " << check_position.z << std::endl;
    //Cube* start_cube = new Cube(Transform{ start_point, glm::vec3(2.0f, .5f, .5f), glm::vec3(0.0f) });
    //Cube* end_cube = new Cube(Transform{ end_point, glm::vec3(2.0f, .5f, .5f), glm::vec3(0.0f) });
    //objects_.push_back(start_cube);
    //objects_.push_back(end_cube);



    //for (unsigned int i = 0; i < matched_triangles.size(); ++i) {
    //    /*std::cout << "triangle>> p1:"; // print triangles of matched triangle
    //    std::cout << matched_triangles[i]->GetPoints()[0].x << "," << matched_triangles[i]->GetPoints()[0].y << "," << matched_triangles[i]->GetPoints()[0].z << " //p2: ";
    //    std::cout << matched_triangles[i]->GetPoints()[1].x << "," << matched_triangles[i]->GetPoints()[1].y << "," << matched_triangles[i]->GetPoints()[1].z << " //p3: ";
    //    std::cout << matched_triangles[i]->GetPoints()[2].x << "," << matched_triangles[i]->GetPoints()[2].y << "," << matched_triangles[i]->GetPoints()[2].z << std::endl;*/
    //    Triangle * check_triangle = matched_triangles[i];
    //    glm::vec3 reflect_position = ReflectedPointOnTriangle( check_triangle, check_position);

    //    
    //    Cube* reflect_cube = new Cube(Transform{ reflect_position, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f) }, ray_shader_);
    //    objects_.push_back(reflect_cube);
    //    std::cout << "reflect position: " << reflect_position.x << ", " << reflect_position.y << ", " << reflect_position.z << std::endl;/**/
    //    
    //    /* reflected points */
    //    glm::vec3 direction_to = glm::normalize(end_point - reflect_position);
    //    float distance_reflect_to_end = glm::distance(reflect_position, end_point);
    //    Ray * ray = new Ray( reflect_position, direction_to, ray_shader_ );
    //    ray->InitializeRay(distance_reflect_to_end);
    //    //objects_.push_back(ray);
    //    float temp_t;
    //    std::set<std::pair<float,Triangle*>> hit_triangles;
    //    if (map_->IsHit(*ray, hit_triangles)) {
    //        // hit at the back because we don't care if 
    //        /*if (std::get<1>(*hit_triangles.end()) == check_triangle)
    //        {
    //            std::cout << "It's reflected!!!" << std::endl;
    //        }
    //        else std::cout << "It hit reflected on the object but got hit something else " << std::endl;*/
    //        bool is_check_triangle_hit_by_reflect_point = false;
    //        float distance_to_check_triangle_from_reflect = 0;
    //        float is_hit_something_else = false;
    //        for (auto& [distance, triangle] : hit_triangles) {
    //            if (triangle == check_triangle) {
    //                distance_to_check_triangle_from_reflect = distance;
    //                is_check_triangle_hit_by_reflect_point = true;
    //                break;
    //            }
    //        }
    //        if (is_check_triangle_hit_by_reflect_point)
    //        {
    //            for (auto& [distance, triangle] : hit_triangles) {
    //                if (distance > distance_to_check_triangle_from_reflect
    //                    && distance < distance_reflect_to_end) {
    //                    is_hit_something_else = true;
    //                    std::cout << "Hit at distance: " << distance << std::endl;

    //                }
    //            }
    //            if (is_hit_something_else) {
    //                std::cout << "it hit something else, sorry. " << std::endl;
    //            }
    //            else {
    //                // it reflected
    //                std::cout << "it reflects" << std::endl;
    //                glm::vec3 hit_position = reflect_position + direction_to * distance_to_check_triangle_from_reflect;

    //                Cube * reflect_cube = new Cube(Transform{ hit_position, glm::vec3(.3f, .3f, .3f), glm::vec3(0.0f) }, ray_shader_);
    //                objects_.push_back(reflect_cube);

    //                Ray* from_reflect = new Ray(hit_position, direction_to, ray_shader_);
    //                from_reflect->InitializeRay(glm::distance(hit_position, end_point));
    //                objects_.push_back(from_reflect);

    //                Ray* to_triangle = new Ray(start_point, glm::normalize(hit_position - start_point), ray_shader_);
    //                to_triangle->InitializeRay(glm::distance(hit_position, start_point));
    //                objects_.push_back(to_triangle);

    //                // store only hit point!!
    //            }
    //        }
    //        else {
    //            std::cout << "N0!! it does't reflect anything" << std::endl;
    //        }
    //    }
    //    else {
    //        std::cout << "This ray doesn't hit anything" << std::endl;
    //    }

    //}

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

void RayTracer::ScanHit(Point * point)
{
    glm::vec4 direction = { 1.0f , 0.0f, 0.0f, 1.0f }; // initial scan direction
    float scan_precision = 0.5;
    for (float i = 0; i < 360; i += scan_precision)
        for (float j = 0; j < 360; j += scan_precision) {
            auto trans_direction = glm::rotate(glm::mat4(1.0f), glm::radians(i), glm::vec3(0.0f, 1.0f, 0.0f));
            trans_direction = glm::rotate(trans_direction, glm::radians(j), glm::vec3(0.0f, 0.0f, 1.0f));
            auto new_direction = trans_direction * direction;
            glm::vec3 i_direction = glm::vec3{ new_direction.x, new_direction.y, new_direction.z };
            Ray * ray = new Ray ( point->position, i_direction );
            Triangle * hit_triangle = nullptr;
            float hit_distance; // doesnt do anything yet // maybe implement later. 
            if (map_->IsHit( * ray, hit_distance, hit_triangle)) {
                point->hit_triangles[hit_triangle] = true;
            }
            delete ray;
        }
}


void RayTracer::Trace(Point * start_point, Point * end_point)
{

    // check if already calculated
    if (!start_point->neighbour_record[end_point].empty()) return;


    //
    glm::vec3 start_position = start_point->position;
    glm::vec3 end_position = end_point->position;

    // Debug the point
    // Cube* start_cube = new Cube(Transform{ start_position, glm::vec3(.5f, 0.5f, .5f), glm::vec3(0.0f) });
    // Cube* end_cube = new Cube(Transform{ end_position, glm::vec3(.5f, 0.5f, .5f), glm::vec3(0.0f) });
    // objects_.push_back(start_cube);
    // objects_.push_back(end_cube);

    // find if LOS
    if (IsDirectHit(start_point->position, end_point->position)) {
        start_point->neighbour_record[end_point].push_back(direct_record_); //start_point has LOS with end_point
        end_point->neighbour_record[start_point].push_back(direct_record_); //end_point has LOS with start_point
    
        // Debug the direct path
        //Ray * ray = new Ray(start_position, glm::normalize(end_position-start_position));
        //ray->InitializeRay(glm::distance(start_position, end_position));
        //objects_.push_back(ray);
    }
    else {
        // Implement multiple knife-edge diffraction
        std::vector<glm::vec3> edges_points;

        if (IsKnifeEdgeDiffraction(start_point->position, end_point->position, edges_points)) {
            // Record the diffraction points
        }
    }

    // find possible reflections
    std::vector <glm::vec3> reflected_points;
    if (IsReflected(start_point, end_point, reflected_points)) {
        //std::cout << "Reflected!!" << std::endl;
        Record* saving_record = new Record(RecordType::kReflect, reflected_points);
        records_.push_back(saving_record);
        start_point->neighbour_record[end_point].push_back(saving_record);
        end_point->neighbour_record[start_point].push_back(saving_record);
        // Debug the reflection
        /*for (auto reflected_position : reflected_points) {
            Cube * reflected_point = new Cube(Transform{ reflected_position, glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f) });
            Ray * start_to_point_ray = new Ray(start_position, glm::normalize(reflected_position - start_position));
            start_to_point_ray->InitializeRay(glm::distance(start_position, reflected_position));
            Ray* point_to_end_ray = new Ray(reflected_position, glm::normalize(end_position - reflected_position));
            point_to_end_ray->InitializeRay(glm::distance(reflected_position, end_position));

            objects_.push_back(reflected_point);
            objects_.push_back(start_to_point_ray);
            objects_.push_back(point_to_end_ray);
        }*/
    }
    
}

bool RayTracer::IsDirectHit(glm::vec3 start_point, glm::vec3 end_point) const
{
    // get direction from start point to end point
    glm::vec3 origin = start_point;
    glm::vec3 direction = glm::normalize(end_point - origin);
    float start_to_end_distance = glm::distance(start_point, end_point);
    Ray ray{ origin, direction };
    float distance = -1;
    // trace the ray on this direction 
    // check if the direction hit something and the t is not betwen start_point to end_point length
    if (map_->IsHit(ray, distance) && distance != -1 && distance < start_to_end_distance) {
        //std::cout << "hit something at distance: " << distance << std::endl;
        return false;
    }
    return true;
}

bool RayTracer::IsReflected(Point * start_point, Point * end_point, std::vector<glm::vec3> & reflected_points)  /// TODO: add const 
{
    // search the hitable triangles between two points
    glm::vec3 start_position = start_point->position;
    glm::vec3 end_position = end_point->position;
    // match the co-exist triangles between two points
    std::vector<Triangle*> matched_triangles;
    /// search matches triangles
    for (auto const& [triangle, exist_value] : start_point->hit_triangles)
        if (end_point->hit_triangles[triangle] == true) matched_triangles.push_back(triangle);
   
    // check the reflection points on matches triangles
    for (Triangle * matched_triangle : matched_triangles) {
        // std::cout << "matched triangle: " << matched_triangle << std::endl;;
        // reflect one of the point on the triangle plane
        glm::vec3 reflected_position = ReflectedPointOnTriangle(matched_triangle, start_position);
        // Debug reflect points
        //    Cube* reflect_box = new Cube(Transform{ reflected_position, glm::vec3(.1f, .1f, .1f), glm::vec3(0.0f) });
        //    objects_.push_back(reflect_box);
        // trace from the reflected point 
        glm::vec3 ref_to_end_direction = glm::normalize(end_position - reflected_position);
        float ref_to_end_distance = glm::distance(reflected_position, end_position);
        Ray* ref_to_end_ray = new Ray(reflected_position, ref_to_end_direction);
        std::set<std::pair<float, Triangle*>> hit_triangles; // hit triangles from reflected_position to end_position
        if (map_->IsHit(*ref_to_end_ray, hit_triangles)) {
            //std::cout << "hit some triangles" << std::endl;
            // check if the reflect point hit the matched triangle;
            bool is_hit_matched_triangle = false;
            float distance_to_triangle = -1;
            bool direct_hit = true;
            //std::cout << "distance: ";
            for (auto const& [distance, triangle] : hit_triangles) {
                if (triangle == matched_triangle) {
                    is_hit_matched_triangle = true; 
                    distance_to_triangle = distance;
                }
                //if(distance_to_triangle > -1) std::cout << "ref_to_triangle: " << distance_to_triangle << "//hit the distance: " << distance << "//ref_to_end: " << ref_to_end_distance << std::endl;
                if (is_hit_matched_triangle && distance < ref_to_end_distance && distance > distance_to_triangle) {
                    direct_hit = false;
                    //std::cout << "doesn't reach point " << std::endl;
                    break;
                }
            }
            std::cout << std::endl;
            // check if the reflected point 
            if (direct_hit && is_hit_matched_triangle == true) {
                glm::vec3 point_on_triangle = reflected_position + ref_to_end_direction * distance_to_triangle;
                glm::vec3 start_to_triangle_direction = glm::normalize(point_on_triangle - start_position);
                float start_to_triangle_distance = glm::distance(start_position, point_on_triangle);
                Ray * ray_to_triangle = new Ray(start_position, start_to_triangle_direction);
                // scan if the start point hit something before the reflected point
                float nearest_hit_distance = -1.0f;
                if (map_->IsHit(*ray_to_triangle, nearest_hit_distance) &&
                    nearest_hit_distance < start_to_triangle_distance) {
                    continue; 
                }
                else {
                    //Ray* triangle_to_end_ray = new Ray(end_position, -ref_to_end_direction);
                    //triangle_to_end_ray->InitializeRay(glm::distance(end_position, point_on_triangle));
                    //ray_to_triangle->InitializeRay(start_to_triangle_distance);
                    //Cube* reflect_on_triangle_point = new Cube(Transform{ point_on_triangle , glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.0f) });
                    //objects_.push_back(reflect_on_triangle_point);
                    //objects_.push_back(triangle_to_end_ray);
                    //objects_.push_back(ray_to_triangle);
                    //std::cout << "nearest hit: " << nearest_hit_distance << ", start_to_tri " << start_to_triangle_distance << std::endl;
                    reflected_points.push_back(point_on_triangle);
                    //store the reflect point on the triangle
                };
                
                // TODO: delete ray_to_triangle after checking
                delete ray_to_triangle;
                // store the reflection point
            }
        }
        delete ref_to_end_ray;
    }
   
    if (reflected_points.size() == 0) return false;
    return true;
}

bool RayTracer::IsOutdoor(glm::vec3 start_point) const
{
    return false;
}

glm::vec3 RayTracer::ReflectedPointOnTriangle(Triangle * triangle, glm::vec3 points) /// TODO change to const later
{
    /// algorithms
  
    // 1. construct the plane from 3 points (non-collinear points)
    glm::vec3 p_0 = triangle->GetPoints()[0];
    glm::vec3 p_1 = triangle->GetPoints()[1];
    glm::vec3 p_2 = triangle->GetPoints()[2];
    /*Cube* check_tr_1 = new Cube(Transform{ glm::vec3{p_0.x,p_0.y,p_0.z}, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f) });
    Cube* check_tr_2 = new Cube(Transform{ glm::vec3{p_1.x,p_1.y,p_1.z}, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f) });
    Cube* check_tr_3 = new Cube(Transform{ glm::vec3{p_2.x,p_2.y,p_2.z}, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f) });
    objects_.push_back(check_tr_1);
    objects_.push_back(check_tr_2);
    objects_.push_back(check_tr_3); */ // display the corner of hit triangles.

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

bool RayTracer::IsKnifeEdgeDiffraction(glm::vec3 start_point, glm::vec3 end_point, std::vector<glm::vec3>& edges_points)
{
    return false;
}

void RayTracer::DrawObjects(Camera * main_camera) const
{
    for (auto & object : objects_) {
        object->DrawObject(main_camera);
    }
}

Point * RayTracer::InitializeOrCallPoint(glm::vec3 initialized_point)
{
    if (points_[initialized_point] == nullptr) {
        Point*  point = new Point(initialized_point);
        ScanHit(point);
        points_[initialized_point] = point;
        return point;
    }
    return points_[initialized_point];
}
