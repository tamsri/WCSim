#include "ray_tracer.hpp"

#include <iostream>
#include <set>
#include <utility>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <stack>

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
    srand(0);
    Point* transmitter_point = InitializeOrCallPoint(glm::vec3(0.0f,10.0f, 0.0f));
    glm::vec3 end_position = glm::vec3(40.0f, 5.00F, 6.0f);
    
    std::vector<Point*> points;
    for (unsigned int i = 0; i < 10; ++i) {
        
        std::chrono::steady_clock::time_point start_init_time = std::chrono::steady_clock::now();
        Point * current_receiver = InitializeOrCallPoint(glm::vec3(rand()%200-100.0f, rand()%10+1.0f, rand() % 200 - 100.0f));

        //Point* current_receiver = InitializeOrCallPoint(glm::vec3(60.0f, 35.0f, -40.0f));

        std::chrono::steady_clock::time_point end_init_time = std::chrono::steady_clock::now();

        std::chrono::steady_clock::time_point start_trace_time = std::chrono::steady_clock::now();
        Trace(transmitter_point, current_receiver);
        std::chrono::steady_clock::time_point end_trace_time = std::chrono::steady_clock::now();

        

        std::chrono::steady_clock::time_point start_draw_time = std::chrono::steady_clock::now();
        InitializeDrawPointsComponents(transmitter_point, current_receiver);
        std::chrono::steady_clock::time_point end_draw_time = std::chrono::steady_clock::now();

        float total_attenuation_in_dB;
        float freq = 2.5f; // in GHz
        std::chrono::steady_clock::time_point start_cal_time = std::chrono::steady_clock::now();
        CalculatePathLoss(transmitter_point, current_receiver, total_attenuation_in_dB, freq);
        std::chrono::steady_clock::time_point end_cal_time = std::chrono::steady_clock::now();

        std::cout << "total path loss : " << total_attenuation_in_dB << std::endl;
        std::cout << "distance: " << glm::distance(current_receiver->position, transmitter_point->position) << std::endl;
        std::cout << "Receiver Number " << i << std::endl;
        std::cout << "Init Tracer took " << std::chrono::duration_cast<std::chrono::microseconds>(end_init_time - start_init_time).count() / 1e3 << " ms.\n";
        std::cout << "Ray Tracer took " << std::chrono::duration_cast<std::chrono::microseconds>(end_trace_time - start_trace_time).count() / 1e3 << " ms.\n";
        std::cout << "Initialize Draw took " << std::chrono::duration_cast<std::chrono::microseconds>(end_draw_time - start_draw_time).count() / 1e3 << " ms.\n";
        std::cout << "Calculation took " << std::chrono::duration_cast<std::chrono::microseconds>(end_cal_time - start_cal_time).count() << " us.\n";
        std::cout << "-----------------" << std::endl;
        points.push_back(current_receiver);
    }
    //Point* end_point_2 = InitializeOrCallPoint(end_position_2);
    // Test the trace function
    //Trace(start_point, end_point);

    
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
    float scan_precision = 10.0f;
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
    Cube* start_cube = new Cube(Transform{ start_point->position,glm::vec3(0.2f, 10.0f, 0.2f), glm::vec3(0.0f) });
    Cube* end_cube = new Cube(Transform{ end_point->position,glm::vec3(0.5f, .5f, 0.5f), glm::vec3(0.0f) });
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
            direct_ray->SetRayColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
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

                start_to_point_ray->SetRayColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
                point_to_end_ray->SetRayColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

                objects_.push_back(reflected_point);
                objects_.push_back(start_to_point_ray);
                objects_.push_back(point_to_end_ray);
            }
        }
            break;
        case RecordType::kEdgeDiffraction: {
            
            auto edges_points = record->data;
            std::stack<glm::vec3> edges_from_start;
            edges_from_start.push(start_position);
            std::stack<glm::vec3> edges_from_end;
            edges_from_end.push(end_position);
            std::cout << "edges: " << edges_points.size() << std::endl;
            glm::vec4 color = (edges_points.size() >= 5)? glm::vec4(1.00f, 0.50f, 1.00f, 1.00f) : glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
            // separate the edges to each points
            while (!edges_points.empty()) {
                edges_from_start.push(NearestEdgeFromPoint(edges_from_start.top(), edges_points));
                if (edges_points.empty()) break;
                edges_from_end.push(NearestEdgeFromPoint(edges_from_end.top(), edges_points));
            }
            /* *-------*-------*   */
            if (edges_from_start.size() != edges_from_end.size()) {

                glm::vec3 center_edge_position = edges_from_start.top();
                edges_from_start.pop();
                Ray* start_to_center_ray = new Ray(center_edge_position, glm::normalize(edges_from_start.top()-center_edge_position));
                start_to_center_ray->InitializeRay(glm::distance(edges_from_start.top(), center_edge_position));

                Ray* end_to_center_ray = new Ray(center_edge_position, glm::normalize(edges_from_end.top()-center_edge_position));
                end_to_center_ray->InitializeRay(glm::distance(edges_from_end.top(), center_edge_position));

                Cube* center_point = new Cube(Transform{ center_edge_position, glm::vec3(0.1f), glm::vec3(0.0f) });

                start_to_center_ray->SetRayColor(color);
                end_to_center_ray->SetRayColor(color);

                objects_.push_back(start_to_center_ray);
                objects_.push_back(end_to_center_ray);
                objects_.push_back(center_point);
                if (edges_from_start.size() < 2) continue; // already done, go back

            }
            /* *------------* */
            else {
                Ray* between_ray = new Ray(edges_from_start.top(), glm::normalize(edges_from_end.top() - edges_from_start.top()));
                between_ray->InitializeRay(glm::distance(edges_from_start.top(), edges_from_end.top()));
                between_ray->SetRayColor(color);
                objects_.push_back(between_ray);
            }

            
            while (edges_from_start.size()>1) {

                glm::vec3 begin_at_start = edges_from_start.top();
                glm::vec3 begin_at_end = edges_from_end.top();
                edges_from_start.pop();
                edges_from_end.pop();
                Ray* from_start_ray = new Ray(begin_at_start, glm::normalize(edges_from_start.top() - begin_at_start));
                Cube* from_start_point = new Cube(Transform{ begin_at_start, glm::vec3(0.1f), glm::vec3(0.0f) });
                from_start_ray->InitializeRay(glm::distance(begin_at_start, edges_from_start.top()));
                Ray* from_end_ray = new Ray(begin_at_end, glm::normalize(edges_from_end.top() - begin_at_end));
                Cube* from_end_point = new Cube(Transform{ begin_at_end, glm::vec3(0.1f), glm::vec3(0.0f) });
                from_end_ray->InitializeRay(glm::distance(begin_at_end, edges_from_end.top()));
                
                from_start_ray->SetRayColor(color);
                from_end_ray->SetRayColor(color);

                objects_.push_back(from_start_point);
                objects_.push_back(from_end_point);
                objects_.push_back(from_start_ray);
                objects_.push_back(from_end_ray);

                //edges_from_start.pop();
                //edges_from_end.pop();
            }

        }
            break;
        }
    }
}

bool RayTracer::CalculatePathLoss(Point* transmitter_point, Point* receiver_point, float& total_attenuation_in_dB, float frequency_in_GHz)
{

    const glm::vec3 transmitter_position = transmitter_point->position;
    const glm::vec3 receiver_position = receiver_point->position;
    auto records = transmitter_point->neighbour_record[receiver_point];
    if (records.size() == 0) return false;

    const float c = 3e8;
    const float frequency = frequency_in_GHz*1e9;
    const float wave_length = c / frequency;
    float total_attenuation_in_lin = 0.0f;
    const float pi = atan(1.0f) * 4;
    const float coff= 0.8f;

    float direct_att_in_lin = 0.0f; 
    float direct_transmitter_gain = 0.0f;
    float ref_att_in_lin = 0.0f;
    float ref_transmitter_gain = 0.0f;
    float diffraction_in_dB = 0.0f;
    float diff_transmitter_gain = 0.0f;

    for (auto record : records) {
        switch (record->type) {
        case RecordType::kDirect: {
            const float distance = glm::distance(transmitter_position, receiver_position);
            // todo : convert to dB
            direct_att_in_lin = 1/pow(4*pi*distance*frequency/c, 2) ;
            /*std::cout << "direct path loss in lin: " << direct_att_in_lin << std::endl;
            std::cout << "direct path loss in dB: " << 10.0f*log10(direct_att_in_lin) << std::endl;*/
            /// TODO: Calculate Transmitter Gain

        }
        break;
        case RecordType::kReflect: {
            for (auto point : record->data) {
                float d1 = glm::distance(point, transmitter_position);
                float d2 = glm::distance(point, receiver_position);
                // todo : convert to dB
                ref_att_in_lin += 1/(pow(4*pi*(d1+d2)*frequency/(c), 2)/coff)*0.8f;
                /*std::cout << "reflection position: " << point.x << ", " << point.y << ", " << point.z << std::endl;
                std::cout << "d1: " << d1 << ", d2: " << d2 << std::endl;
                std::cout << "reflect path loss in lin: " << ref_att_in_lin << std::endl;
                std::cout << "reflect path loss in dB: " << 10.0f * log10(ref_att_in_lin) << std::endl;*/
                /// TODO: Calculate Transmitter Gain
            }
        }
        break;
        case RecordType::kEdgeDiffraction: {
            if (record->data.size() == 1) {
                // Single Edge Diffraction Calculation
                diffraction_in_dB = CalculateSingleKnifeEdge(transmitter_position, record->data[0], receiver_position, frequency);
                std::cout << "diff in dB: " << diffraction_in_dB << std::endl;
                //total_attenuation_in_dB += diffraction_los;
            }
            else if (record->data.size() == 2) {
                // Double Edge Diffraction Calculation
                float max_v = std::max(
                                CalculateVOfEdge(transmitter_position, record->data[0], receiver_position, frequency),
                                CalculateVOfEdge(transmitter_position, record->data[1], receiver_position, frequency));
                diffraction_in_dB = CalculateDiffractionByV(max_v);
                std::cout << "diff 2 in dB: " << diffraction_in_dB << std::endl;

            }
            else if (record->data.size() == 3) {
                std::vector<glm::vec3> edges = record->data;
                glm::vec3 near_tramsitter_edge_position = NearestEdgeFromPoint(transmitter_position, edges);
                glm::vec3 center_edge_position = NearestEdgeFromPoint(near_tramsitter_edge_position, edges);
                glm::vec3 near_receiver_edge_position = NearestEdgeFromPoint(receiver_position, edges);
            
                float c1 = CalculateSingleKnifeEdge(transmitter_position, near_tramsitter_edge_position, receiver_position, frequency);
                float c2 = CalculateSingleKnifeEdge(transmitter_position, center_edge_position, receiver_position, frequency);
                float c3 = CalculateSingleKnifeEdge(transmitter_position, near_receiver_edge_position, receiver_position, frequency);
                
                std::pair<float, float> correction_cosines;
                CalculateCorrectionCosines(transmitter_position, record->data, receiver_position, correction_cosines);
                float c_1_cap = (6 - c2 + c1) * correction_cosines.first;
                float c_2_cap = (6 - c2 + c3) * correction_cosines.second;
                diffraction_in_dB = c2 + c1 + c3 - c_1_cap - c_2_cap;

            }
            else {
                std::set<float> highest_v;
                std::map<float, glm::vec3> edges_dict;
                for (auto edge : record->data) {
                    float temp_v = CalculateVOfEdge(transmitter_position, edge, receiver_position, frequency);
                    edges_dict[temp_v] = edge;
                    highest_v.insert(temp_v);
                }
                std::vector<glm::vec3> edges; int i = 0;
                for (auto ritr = highest_v.rbegin(); ritr != highest_v.rend(); ++ritr) {
                    edges.push_back(edges_dict[*ritr]);
                    //std::cout << "v: " << *ritr << std::endl;
                    if (i++ == 2) break;
                }
                //std::cout << "edges: " << edges[0].x << ", " << edges[0].y << ", " << edges[0].z << std::endl;
                //std::cout << "edges: " << edges[1].x << ", " << edges[1].y << ", " << edges[1].z << std::endl;
                //std::cout << "edges: " << edges[2].x << ", " << edges[2].y << ", " << edges[2].z << std::endl;
                glm::vec3 near_tramsitter_edge_position = NearestEdgeFromPoint(transmitter_position, edges);
                glm::vec3 center_edge_position = NearestEdgeFromPoint(near_tramsitter_edge_position, edges);
                glm::vec3 near_receiver_edge_position = NearestEdgeFromPoint(receiver_position, edges);

                float c1 = CalculateSingleKnifeEdge(transmitter_position, near_tramsitter_edge_position, receiver_position, frequency);
                float c2 = CalculateSingleKnifeEdge(transmitter_position, center_edge_position, receiver_position, frequency);
                float c3 = CalculateSingleKnifeEdge(transmitter_position, near_receiver_edge_position, receiver_position, frequency);

                std::pair<float, float> correction_cosines;
                CalculateCorrectionCosines(transmitter_position, record->data, receiver_position, correction_cosines);
                float c_1_cap = (6 - c2 + c1) * correction_cosines.first;
                float c_2_cap = (6 - c2 + c3) * correction_cosines.second;
                diffraction_in_dB = c2 + c1 + c3 - c_1_cap - c_2_cap;
            }

        }
        break;
        }
    }
    std::cout << "direct_att_in_lin: " << direct_att_in_lin << std::endl;
    std::cout << "ref_att_in_lin: " << ref_att_in_lin << std::endl;
    std::cout << "diffraction_in_dB: " << diffraction_in_dB << std::endl;
    total_attenuation_in_dB = 10.0f*log10(direct_att_in_lin+0.8f* ref_att_in_lin )  ;
    return true;
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
    const glm::vec3 start_position = start_point->position;
    const glm::vec3 end_position = end_point->position;
    // try multiple edges scanning
    unsigned int max_scan = 5;
    unsigned int current_scan = 0;
    glm::vec3 left_position = start_position;
    glm::vec3 right_position = end_position;
    while (!IsDirectHit(left_position, right_position) && current_scan < max_scan) {
        current_scan++;
        glm::vec3 edge_from_left_position;
        if (!FindEdge(left_position, right_position, edge_from_left_position)) return false;
        glm::vec3 edge_from_right_position;
        if (!FindEdge(right_position, left_position, edge_from_right_position)) return false;

        if (IsDirectHit(edge_from_left_position, edge_from_right_position)) {
            if (glm::distance(edge_from_left_position, edge_from_right_position) < 0.5f) {
                edges_points.push_back((edge_from_left_position + edge_from_right_position) / 2.0f);
                CleanEdgePoints(edges_points);
                return true;
            }
            edges_points.push_back(edge_from_left_position);
            edges_points.push_back(edge_from_right_position);
            CleanEdgePoints(edges_points);
            return true;
        }
        // search more eges
        left_position = edge_from_left_position;
        right_position = edge_from_right_position;
        edges_points.push_back(edge_from_left_position);
        edges_points.push_back(edge_from_right_position);
        CleanEdgePoints(edges_points);
    }
    return false;
}

bool RayTracer::FindEdge(glm::vec3 start_position, glm::vec3 end_position, glm::vec3& edge_position)
{
    const glm::vec3 up_direction = glm::vec3(0.0f, 1.0f, 0.0f);
    const float scan_precision = 0.05f;
    glm::vec3 start_end_direction = glm::normalize(end_position - start_position);
    
    const float min_x = std::min(end_position.x, start_position.x);
    const float max_x = std::max(end_position.x, start_position.x);
    const float min_z = std::min(end_position.z, start_position.z);
    const float max_z = std::max(end_position.z, start_position.z);

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
        if (glm::degrees(glm::angle(scan_direction, up_direction)) < .5f) return false;

        //glm::vec3 scan_direction = glm::vec3(0.0f, 1.0f, 0.0f);
        Ray scan_ray{ start_position, scan_direction }; //implement to heap later

        if (map_->IsHit(scan_ray, scan_hit_distance)) {
            latest_hit_position = start_position + scan_direction * scan_hit_distance;
            //lastest_hit_position should be in between start_positon and end_position in xz plane
            if (latest_hit_position.x < min_x || latest_hit_position.x > max_x ||
                latest_hit_position.z < min_z || latest_hit_position.z > max_z) {
                //break;
                //std::cout << min_x << " < " << latest_hit_position.x << " < " << max_x << std::endl;
                break;
            }
            latest_hit_distance = scan_hit_distance;
            latest_hit_direction = scan_direction;
        }
        else {
            // delete scan_ray;
            if (latest_hit_distance == -1.0f) return false; // doesn't hit anything since the first place
            break;
        }
    }

    //float distance_to_obstacle_on_xz = cos();
    glm::vec3 start_end_on_xz_direction = glm::normalize(glm::vec3(start_end_direction.x, 0.0f, start_end_direction.z));
    double start_end_to_on_xz_angle = glm::angle(latest_hit_direction, start_end_on_xz_direction);
    float distance_on_xz = latest_hit_distance * cos(start_end_to_on_xz_angle);
    double start_edge_angle = glm::angle(start_end_on_xz_direction, scan_direction);
    float distance_to_edge = distance_on_xz / cos(start_edge_angle);
    //std::cout << "distance : " << distance_to_edge << std::endl;
    // calculate the start_edge_point after exit the obstables
    edge_position = start_position + scan_direction * distance_to_edge;
    return true;
}

glm::vec3 RayTracer::NearestEdgeFromPoint(glm::vec3 point_position, std::vector<glm::vec3> & edges_points)
{
    std::map<float, glm::vec3> distance_from_point;
    for (auto edge_point : edges_points) { // implemenet to function
        glm::vec3 point_positon_on_xz = glm::vec3(point_position.x, 0.0f, point_position.z);
        glm::vec3 edge_point_on_xz = glm::vec3(edge_point.x, 0.0f, edge_point.z);
        distance_from_point[glm::distance(edge_point_on_xz, point_positon_on_xz)] = edge_point;
    }
    edges_points.erase(std::remove(edges_points.begin(), edges_points.end(), distance_from_point.begin()->second), edges_points.end());
    return glm::vec3(distance_from_point.begin()->second);
}

void RayTracer::CleanEdgePoints(std::vector<glm::vec3>& edges_points)
{
    if (edges_points.size() == 1) return;

    for (int i = edges_points.size()-1; i >=0; --i) 
        for (int j = i - 1; j >= 0; --j) {
            glm::vec3 i_edge = edges_points[i];
            glm::vec3 & j_edge = edges_points[j];
            if (glm::distance(i_edge, j_edge) <= .5f) { 
                j_edge = (i_edge + j_edge) / 2.0f;
                edges_points.pop_back();
                break;
            };
        }
}

float RayTracer::CalculateSingleKnifeEdge(glm::vec3 start_position, glm::vec3 edge_position, glm::vec3 end_position, float frequency)
{
    float v = CalculateVOfEdge(start_position, edge_position, end_position, frequency);
    return CalculateDiffractionByV(v);
}

float RayTracer::CalculateDiffractionByV(float v) {
    return 6.9f + 20.0 * log10(sqrt(pow(v - 0.1, 2) + 1) + v - 0.1);
}
float RayTracer::CalculateVOfEdge(glm::vec3 start_position, glm::vec3 edge_position, glm::vec3 end_position, float frequency){
    float wave_length = 3e8 / (frequency);

    glm::vec3 start_to_end_direction = glm::normalize(end_position - start_position);

    glm::vec3 start_to_edge_direction = glm::normalize(edge_position - start_position);
    glm::vec3 end_to_edge_direction = glm::normalize(edge_position - end_position);
    float angle_1 = glm::angle(start_to_edge_direction, start_to_end_direction);
    float angle_2 = glm::angle(-end_to_edge_direction, start_to_end_direction);
    float r1 = glm::distance(start_position, end_position);
    float r2 = glm::distance(end_position, edge_position);
    float s1 = r1 * cos(angle_1);
    float s2 = r2 * cos(angle_2);
    float h = sin(angle_1) * r1;

    return sqrt(2.0f * (s1 + s2) / (wave_length * r1 * r2)) * h;
}
void RayTracer::CalculateCorrectionCosines(glm::vec3 start_position, std::vector<glm::vec3> edges, glm::vec3 end_position, std::pair<float, float>& calculated_cosines)
{
    glm::vec3 p1 = start_position;
    glm::vec3 p2 = NearestEdgeFromPoint(p1, edges);
    glm::vec3 p3 = NearestEdgeFromPoint(p2, edges);
    glm::vec3 p4 = NearestEdgeFromPoint(p3, edges);
    glm::vec3 p5 = end_position;

    float d1 = glm::distance(glm::vec2(p1.x, p1.z), glm::vec2(p2.x, p2.z));
    float d2 = glm::distance(glm::vec2(p2.x, p2.z), glm::vec2(p3.x, p3.z));
    float d3 = glm::distance(glm::vec2(p3.x, p3.z), glm::vec2(p4.x, p4.z));
    float d4 = glm::distance(glm::vec2(p4.x, p4.z), glm::vec2(p5.x, p5.z));

    calculated_cosines.first = sqrt((d1*(d3+d4))/((d1+d2)*(d2+d3+d4))) ;
    calculated_cosines.second = sqrt((d4*(d1+d2))/((d3+d4)*(d1+d2+d3)));
    return;
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
        if(point->hit_triangles.empty()) ScanHit(point);
        points_[initialized_point] = point;
        return point;
    }
    return points_[initialized_point];
}
