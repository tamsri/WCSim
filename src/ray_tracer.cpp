#include "ray_tracer.hpp"

#include <iostream>
#include <set>
#include <utility>
#include <chrono>
#include <algorithm>
#include <cmath>

#include <glm/gtx/vector_angle.hpp>

#include "object.hpp"
#include "cube.hpp"
#include "ray.hpp"
#include "triangle.hpp"
#include "polygon_mesh.hpp"
#include "shader.hpp"


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
    glm::vec3 start_position = glm::vec3(-15.00f, 10.00f, 40.0f);
    glm::vec3 end_position = glm::vec3(20.0f, 3.00F, 6.0f);
    glm::vec3 end_position_2 = glm::vec3(10.0f, 10.0f, 20.0f);
    Point* start_point = InitializeOrCallPoint(start_position);
    Point* end_point = InitializeOrCallPoint(end_position);
    //Point* end_point_2 = InitializeOrCallPoint(end_position_2);
    // Test the trace function
    //Trace(start_point, end_point);
    std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
    Trace(start_point, end_point);
    std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
    std::cout << "Ray Tracer took " 
                << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count()/1e3 << " ms.\n";
    InitializeDrawPointsComponents(start_point, end_point);

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
            glm::vec3 i_direction = glm::vec3(new_direction);

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
    if (!start_point->neighbour_record[end_point].empty()) return;

    glm::vec3 start_position = start_point->position;
    glm::vec3 end_position = end_point->position;

    // find if LOS
    if (IsDirectHit(start_point->position, end_point->position)) {
        start_point->neighbour_record[end_point].push_back(direct_record_); //start_point has LOS with end_point
        end_point->neighbour_record[start_point].push_back(direct_record_); //end_point has LOS with start_point
    }
    else {
        // Implement multiple knife-edge diffraction
        std::vector<glm::vec3> edges_points;

        if (IsKnifeEdgeDiffraction(start_point, end_point, edges_points)) {
            Record* saving_record = new Record(RecordType::kEdgeDiffraction, edges_points);
            records_.push_back(saving_record);
            start_point->neighbour_record[end_point].push_back(saving_record);
            end_point->neighbour_record[start_point].push_back(saving_record);
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
    }
    
}

void RayTracer::InitializeDrawPointsComponents(Point* start_point, Point* end_point)
{
    Cube* start_cube = new Cube(Transform{ start_point->position,glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.0f) });
    Cube* end_cube = new Cube(Transform{ end_point->position,glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.0f) });
    objects_.push_back(start_cube);
    objects_.push_back(end_cube);
    const glm::vec3 start_position = start_point->position;
    const glm::vec3 end_position = end_point->position;

    auto records = start_point->neighbour_record[end_point];
    for (auto record : records) {
        switch (record->type) {
        case RecordType::kDirect: {
            Ray* direct_ray = new Ray(start_position, glm::normalize(end_position - start_position));
            direct_ray->InitializeRay(glm::distance(start_position, end_position));
            objects_.push_back(direct_ray);
        }
            break;
        case RecordType::kReflect: {
            for (auto reflected_position : record->data) {
                Cube* reflected_point = new Cube(Transform{ reflected_position, glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f) });
                Ray* start_to_point_ray = new Ray(start_position, glm::normalize(reflected_position - start_position));
                start_to_point_ray->InitializeRay(glm::distance(start_position, reflected_position));
                Ray* point_to_end_ray = new Ray(reflected_position, glm::normalize(end_position - reflected_position));
                point_to_end_ray->InitializeRay(glm::distance(reflected_position, end_position));

                objects_.push_back(reflected_point);
                objects_.push_back(start_to_point_ray);
                objects_.push_back(point_to_end_ray);
            }
        }
            break;
        case RecordType::kEdgeDiffraction: {
            auto edges_points = record->data;
            glm::vec3 start_edge_position = NearestEdgeFromPoint(start_position, edges_points);
            glm::vec3 end_edge_position = NearestEdgeFromPoint(end_position, edges_points);

            Cube* start_edge_point = new Cube(Transform{ start_edge_position , glm::vec3(0.1f), glm::vec3(0.0f) });
            Cube* end_edge_point = new Cube(Transform{ end_edge_position , glm::vec3(0.1f), glm::vec3(0.0f) });
            Ray* start_edge_ray = new Ray(start_position, glm::normalize(start_edge_position - start_position));
            start_edge_ray->InitializeRay(glm::distance(start_position, start_edge_position));
            Ray* end_edge_ray = new Ray(end_position, glm::normalize(end_edge_position - end_position));
            end_edge_ray->InitializeRay(glm::distance(end_position, end_edge_position));
            Ray* edge_to_edge_ray = new Ray(start_edge_position, glm::normalize(end_edge_position - start_edge_position));
            edge_to_edge_ray->InitializeRay(glm::distance(start_edge_position, end_edge_position));
            objects_.push_back(start_edge_point);
            objects_.push_back(end_edge_point);
            objects_.push_back(start_edge_ray);
            objects_.push_back(end_edge_ray);
            objects_.push_back(edge_to_edge_ray);
        
        }
            break;
        }
    }

}

bool RayTracer::IsDirectHit(glm::vec3 start_position, glm::vec3 end_position) const
{
    // get direction from start point to end point
    glm::vec3 origin = start_position;
    glm::vec3 direction = glm::normalize(end_position - origin);
    float start_to_end_distance = glm::distance(start_position, end_position);
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
        // reflect one of the point on the triangle plane
        glm::vec3 reflected_position = ReflectedPointOnTriangle(matched_triangle, start_position);
        // trace from the reflected point 
        glm::vec3 ref_to_end_direction = glm::normalize(end_position - reflected_position);
        float ref_to_end_distance = glm::distance(reflected_position, end_position);
        Ray ref_to_end_ray{ reflected_position, ref_to_end_direction };
        std::set<std::pair<float, Triangle*>> hit_triangles; // hit triangles from reflected_position to end_position
        if (map_->IsHit(ref_to_end_ray, hit_triangles)) {
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
                if (is_hit_matched_triangle && distance < ref_to_end_distance && distance > distance_to_triangle) {
                    direct_hit = false;
                    break;
                }
            }
            // check if the reflected point 
            if (direct_hit && is_hit_matched_triangle == true) {
                glm::vec3 point_on_triangle = reflected_position + ref_to_end_direction * distance_to_triangle;
                glm::vec3 start_to_triangle_direction = glm::normalize(point_on_triangle - start_position);
                float start_to_triangle_distance = glm::distance(start_position, point_on_triangle);
                Ray ray_to_triangle{ start_position, start_to_triangle_direction };
                // scan if the start point hit something before the reflected point
                float nearest_hit_distance = -1.0f;
                if (map_->IsHit(ray_to_triangle, nearest_hit_distance) &&
                    nearest_hit_distance < start_to_triangle_distance) {
                    continue; 
                }
                else {
                    reflected_points.push_back(point_on_triangle);
                };
                
            }
        }
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

    glm::vec3 n = triangle->GetNormal();

    float b = (n.x * p_1.x) + (n.y * p_1.y) + (n.z * p_1.z);
    // 2. mirror the point from the plane
    float t = (b - (points.x*n.x + points.y*n.y + points.z*n.z )) / (n.x*n.x + n.y*n.y + n.z*n.z); //distance from point to plane

    /// DEBUG mirror point
    glm::vec3 m = glm::vec3 (points.x + t * n.x, points.y + t * n.y, points.z + t * n.z); // points on mirror

    return  points + 2*t*n; // reverse average point from average point 
}

bool RayTracer::IsKnifeEdgeDiffraction(Point * start_point, Point * end_point, std::vector<glm::vec3>& edges_points)
{
    const float scan_precision = 0.1f;
    
    const glm::vec3 up_direction = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 start_position = start_point->position;
    const glm::vec3 end_position = end_point->position;
    const glm::vec3 start_end_direction = glm::normalize(end_position - start_position);
    const glm::vec3 end_start_direction = -start_end_direction;
    // Scanning Area
    const float min_x = std::min(end_position.x , start_position.x );
    const float max_x = std::max(end_position.x , start_position.x );
    const float min_z = std::min(end_position.z , start_position.z );
    const float max_z = std::max( end_position.z , start_position.z);    

    /*---------- START START-POINT SCANNING ----------*/
    //Scan up from start_point until hit something out of range or no hit
    glm::vec3 latest_hit_position;
    float latest_hit_distance = -1.0f;
    glm::vec3 latest_hit_direction;
    glm::vec3 scan_direction;
    float scan_hit_distance;

    const glm::vec3 start_cross_direction = glm::cross(-up_direction, start_end_direction);
    for (float current_angle = 0.0f; current_angle < 180.0f; current_angle += scan_precision) {
        glm::mat3 direction_trans = glm::rotate(glm::mat4(1.0f), glm::radians(current_angle), start_cross_direction);
        scan_direction = glm::normalize(glm::vec3(direction_trans * start_end_direction));
        // if scan_direction is near almost equal to up_direction, then we stop scanning
        if (glm::degrees(glm::angle(scan_direction, up_direction)) < 1.0f ) return false;

        //glm::vec3 scan_direction = glm::vec3(0.0f, 1.0f, 0.0f);
        Ray scan_ray{ start_position, scan_direction }; //implement to heap later

        if (map_->IsHit(scan_ray, scan_hit_distance)) {
            latest_hit_position = start_position + scan_direction * scan_hit_distance;
            //lastest_hit_position should be in between start_positon and end_position in xz plane
            if (latest_hit_position.x < min_x || latest_hit_position.x > max_x || 
                latest_hit_position.z < min_z || latest_hit_position.z > max_z ) {
                //break;
                std::cout << min_x << " < " << latest_hit_position.x <<" < " << max_x << std::endl ;
                break;
            }
            latest_hit_distance = scan_hit_distance;
            latest_hit_direction = scan_direction;
        } else {
            // delete scan_ray;
            if (latest_hit_distance == -1.0f) return false; // doesn't hit anything since the first place
            break;
        }
    }
    
    //float distance_to_obstacle_on_xz = cos();
    glm::vec3 start_end_on_xz_direction = glm::normalize(glm::vec3(start_end_direction.x, 0.0f, start_end_direction.z));
    double start_end_to_on_xz_angle = glm::angle(latest_hit_direction, start_end_on_xz_direction);
    float distance_on_xz = latest_hit_distance * cos(start_end_to_on_xz_angle);
;
    double start_edge_angle = glm::angle(start_end_on_xz_direction, scan_direction);
    float distance_to_edge = distance_on_xz /cos(start_edge_angle);
    //std::cout << "distance : " << distance_to_edge << std::endl;
    // calculate the start_edge_point after exit the obstables
    glm::vec3 edge_from_start_position = start_position + scan_direction * distance_to_edge;
    
    /*---------- END START POINT SCANNING ----------*/

    
    /*---------- START END-POINT SCANNING ----------*/
    latest_hit_distance = -1.0f;
    //Scan up from end_point until hit something out of range or no hit

    const glm::vec3 end_cross_direction = glm::cross(-up_direction, end_start_direction);
    for (float current_angle = 0.0f; current_angle < 180.0f; current_angle += scan_precision) {
        glm::mat3 direction_trans = glm::rotate(glm::mat4(1.0f), glm::radians(current_angle), end_cross_direction);
        scan_direction = glm::normalize(glm::vec3(direction_trans * end_start_direction));
        // if scan_direction is near almost equal to up_direction, then we stop scanning
        if (glm::degrees(glm::angle(scan_direction, up_direction)) < 1.0f) return false;

        //glm::vec3 scan_direction = glm::vec3(0.0f, 1.0f, 0.0f);
        Ray scan_ray{ end_position, scan_direction }; //implement to heap later

        if (map_->IsHit(scan_ray, scan_hit_distance)) {
            latest_hit_position = end_position + scan_direction * scan_hit_distance;
            //lastest_hit_position should be in between start_positon and end_position in xz plane
            if (latest_hit_position.x < min_x || latest_hit_position.x > max_x ||
                latest_hit_position.z < min_z || latest_hit_position.z > max_z) {
                break;
            }
            latest_hit_distance = scan_hit_distance;
            latest_hit_direction = scan_direction;
        }
        else {
            if (latest_hit_distance == -1.0f) return false; // doesn't hit anything since the first place
            break;
        }
    }
    // calculate the position
    //float distance_to_obstacle_on_xz = cos();
    glm::vec3 end_start_on_xz_direction = glm::normalize(glm::vec3(end_start_direction.x, 0.0f, end_start_direction.z));
    double end_start_to_on_xz_angle = glm::angle(latest_hit_direction, end_start_on_xz_direction);
    float end_distance_on_xz = latest_hit_distance * cos(end_start_to_on_xz_angle);
    
    double end_edge_angle = glm::angle(end_start_on_xz_direction, scan_direction);
    float distance_to_end_edge = end_distance_on_xz / cos(end_edge_angle);
    // calculate the start_edge_point after exit the obstables
    glm::vec3 edge_from_end_position = end_position + scan_direction * distance_to_end_edge;

    /*---------- END END-POINT SCANNING ----------*/

    if (IsDirectHit(edge_from_start_position, edge_from_end_position)) {
        edges_points.push_back(edge_from_start_position);
        edges_points.push_back(edge_from_end_position);
        return true;
    }
    return false;
}

glm::vec3 RayTracer::NearestEdgeFromPoint(glm::vec3 point_position, std::vector<glm::vec3> edges_points)
{
    std::map<float, glm::vec3> distance_from_point;
    for (auto edge_point : edges_points) { // implemenet to function
        glm::vec3 point_positon_on_xz = glm::vec3(point_position.x, 0.0f, point_position.z);
        glm::vec3 edge_point_on_xz = glm::vec3(edge_point.x, 0.0f, edge_point.z);
        distance_from_point[glm::distance(edge_point_on_xz, point_positon_on_xz)] = edge_point;
    }
    return glm::vec3(distance_from_point.begin()->second);
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
