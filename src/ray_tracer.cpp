#include "ray_tracer.hpp"


#include <set>
#include <utility>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <stack>
#include <queue>
#include <thread>

#include <glm/gtx/vector_angle.hpp>

#include "object.hpp"
#include "cube.hpp"
#include "ray.hpp"
#include "triangle.hpp"
#include "polygon_mesh.hpp"

#include "transmitter.hpp"
#include "receiver.hpp"

#include "recorder.hpp"

RayTracer::RayTracer(PolygonMesh* map) :map_(map)
{
}

std::map <Triangle *, bool> RayTracer::ScanHit(const glm::vec3 position) const
{
	std::map<Triangle*, bool> hit_triangles;
	// Approach I: when the triangles are more than the generated scanning rays
	glm::vec4 direction = { 1.0f , 0.0f, 0.0f, 1.0f }; // initial scan direction
	float scan_precision = 1.0f;
	for (float i = 0; i < 360; i = i + scan_precision)
		for (float j = 0; j < 360; j = + scan_precision) {
			auto trans_direction = glm::rotate(glm::mat4(1.0f), glm::radians(i), glm::vec3(0.0f, 1.0f, 0.0f));
			trans_direction = glm::rotate(trans_direction, glm::radians(j), glm::vec3(0.0f, 0.0f, 1.0f));
			auto new_direction = trans_direction * direction;
			auto i_direction = glm::vec3(new_direction);

			Ray ray{ position, i_direction };
			Triangle* hit_triangle = nullptr;
			float hit_distance; // doesnt do anything yet // maybe implement later. 
			if (map_->IsHit(ray, hit_distance, hit_triangle)) {
				hit_triangles[hit_triangle] = true;
			}
		}
	return hit_triangles;
}

std::vector <Triangle*> RayTracer::ScanHitVec(const glm::vec3 position) const
{
	std::vector <Triangle*> hit_triangles;
	// Approach I: when the triangles are more than the generated scanning rays
	glm::vec4 direction = { 1.0f , 0.0f, 0.0f, 1.0f }; // initial scan direction
	float scan_precision = 2.0f;
	for (float i = 0; i < 360; i = i + scan_precision)
		for (float j = 0; j < 360; j = i +scan_precision) {
			auto trans_direction = glm::rotate(glm::mat4(1.0f), glm::radians(i), glm::vec3(0.0f, 1.0f, 0.0f));
			trans_direction = glm::rotate(trans_direction, glm::radians(j), glm::vec3(0.0f, 0.0f, 1.0f));
			auto new_direction = trans_direction * direction;
			auto i_direction = glm::vec3(new_direction);

			Ray ray{ position, i_direction };
			Triangle* hit_triangle = nullptr;
			float hit_distance; // doesnt do anything yet // maybe implement later. 
			if (map_->IsHit(ray, hit_distance, hit_triangle)) {
				hit_triangles.push_back(hit_triangle);
			}
		}
	return hit_triangles;
}


void RayTracer::LineTrace(const glm::vec3 start_position,
                          const glm::vec3 end_position,
                          std::vector<Record> & records) const {
    if(IsDirectHit(start_position, end_position)){
            records.emplace_back(RecordType::kDirect);
    }else{
        std::vector<glm::vec3> edges_points;
        if(IsKnifeEdgeDiffraction(start_position, end_position, edges_points))
            records.emplace_back(RecordType::kEdgeDiffraction, edges_points);
    }
}

void RayTracer::ReflectTrace(const glm::vec3 start_position,
                             const glm::vec3 end_position,
                             std::vector<Record> & records) const {
    std::vector<glm::vec3> reflected_points;
    if (IsReflected(start_position, end_position, reflected_points)) {
        records.emplace_back( RecordType::kReflect, reflected_points );
    }
}

void RayTracer::Trace(const glm::vec3 start_position,
                      const glm::vec3 end_position,
                      std::vector<Record> & records) const
{
	// Multithreading (speed up to 30%)
    std::thread line_tracer (&RayTracer::LineTrace, this, start_position, end_position, std::ref(records));
    std::thread reflect_tracer (&RayTracer::ReflectTrace, this, start_position, end_position, std::ref(records));

    line_tracer.join();
    reflect_tracer.join();
}
void RayTracer::TraceMap(   const glm::vec3 tx_position,
                            const glm::vec3 rx_position,
                            std::vector<Record> & records) const{
    if(IsDirectHit(tx_position, rx_position)){
        records.emplace_back(RecordType::kDirect);
    }else{
        std::vector<glm::vec3> edges_points;
        if(IsKnifeEdgeDiffraction(tx_position, rx_position, edges_points))
            records.emplace_back(RecordType::kEdgeDiffraction, edges_points);
    }

    std::vector<glm::vec3> reflected_points;
    if (IsReflected(tx_position, rx_position, reflected_points)) {
        records.emplace_back( RecordType::kReflect, reflected_points );
    }
}

void RayTracer::GetDrawComponents(const glm::vec3 & start_position, const glm::vec3 & end_position,
                                  std::vector<Record>& records, std::vector<Object*>& objects) const
{
	for (auto record : records) {
		switch (record.type) {
		case RecordType::kDirect: {
			Ray* direct_ray = new Ray(start_position, glm::normalize(end_position - start_position));
			direct_ray->InitializeRay(glm::distance(start_position, end_position));
			direct_ray->SetRayColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
			objects.push_back(direct_ray);
			break;
		}
		case RecordType::kReflect: {
			for (auto reflected_position : record.data) {
				Cube* reflected_point = new Cube(Transform{ reflected_position, glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f) });
				Ray* start_to_point_ray = new Ray(start_position, glm::normalize(reflected_position - start_position));
				start_to_point_ray->InitializeRay(glm::distance(start_position, reflected_position));
				Ray* point_to_end_ray = new Ray(reflected_position, glm::normalize(end_position - reflected_position));
				point_to_end_ray->InitializeRay(glm::distance(reflected_position, end_position));

				start_to_point_ray->SetRayColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
				point_to_end_ray->SetRayColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

				objects.push_back(reflected_point);
				objects.push_back(start_to_point_ray);
				objects.push_back(point_to_end_ray);
			}
			break;
		}
		case RecordType::kEdgeDiffraction: {

			auto edges_points = record.data;
			std::stack<glm::vec3> edges_from_start;
			edges_from_start.push(start_position);
			std::stack<glm::vec3> edges_from_end;
			edges_from_end.push(end_position);
			glm::vec4 color = (edges_points.size() >= 5) ? glm::vec4(1.00f, 0.50f, 1.00f, 1.00f) : glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
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
				Ray* start_to_center_ray = new Ray(center_edge_position, glm::normalize(edges_from_start.top() - center_edge_position));
				start_to_center_ray->InitializeRay(glm::distance(edges_from_start.top(), center_edge_position));

				Ray* end_to_center_ray = new Ray(center_edge_position, glm::normalize(edges_from_end.top() - center_edge_position));
				end_to_center_ray->InitializeRay(glm::distance(edges_from_end.top(), center_edge_position));

				Cube* center_point = new Cube(Transform{ center_edge_position, glm::vec3(0.1f), glm::vec3(0.0f) });

				start_to_center_ray->SetRayColor(color);
				end_to_center_ray->SetRayColor(color);

				objects.push_back(start_to_center_ray);
				objects.push_back(end_to_center_ray);
				objects.push_back(center_point);
				if (edges_from_start.size() < 2) continue; // already done, go back

			}
			/* *------------* */
			else {
				Ray* between_ray = new Ray(edges_from_start.top(), glm::normalize(edges_from_end.top() - edges_from_start.top()));
				between_ray->InitializeRay(glm::distance(edges_from_start.top(), edges_from_end.top()));
				between_ray->SetRayColor(color);
				objects.push_back(between_ray);
			}

			while (edges_from_start.size() > 1) {

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

				objects.push_back(from_start_point);
				objects.push_back(from_end_point);
				objects.push_back(from_start_ray);
				objects.push_back(from_end_ray);
			}
			break;
		}
		}
	}

}

bool RayTracer::CalculatePathLoss(Transmitter* transmitter, Receiver * receiver,
                                  const std::vector<Record>& records,
                                  Result& result) const {
    // Validation of Result
    if (records.empty()) {
        result.is_valid = false;
        return false;
    }
    result.is_valid = true;
    // Initialize the result
    result.total_received_power = 0.0f;
    result.transmit_power = transmitter->GetTransmitPower();

    result.direct.rx_gain = 0.0f;
    result.direct.tx_gain = 0.0f;
    result.direct.direct_loss = 0.0f;
    result.direct.delay = 0.0f;

    result.reflections = {};

    result.diffraction.rx_gain = 0.0f;
    result.diffraction.tx_gain = 0.0f;
    result.diffraction.diffraction_loss = 0.0f;
    result.diffraction.delay = 0.0f;

    // Prepare Threads
    std::vector<std::thread> threads;
    for (auto &record: records) {
        switch (record.type) {
            case RecordType::kDirect: {
                std::thread direct_thread(&RayTracer::CalculateDirectPath, this,
                                          record, std::ref(result),
                                          transmitter, receiver);
                threads.push_back(std::move(direct_thread));
            }
                break;
            case RecordType::kReflect: {
                std::thread reflect_thread(&RayTracer::CalculateReflections, this,
                                           record, std::ref(result),
                                           transmitter, receiver);
                threads.push_back(std::move(reflect_thread));
            }
                break;
            case RecordType::kEdgeDiffraction: {
                std::thread diffract_thread(&RayTracer::CalculateDiffraction, this,
                                            record, std::ref(result),
                                            transmitter, receiver);
                threads.push_back(std::move(diffract_thread));
            } break;
        }

    }

    // Join all threads.
    for (std::thread &thread: threads) {
        if (thread.joinable()) thread.join();
    }

    // Summary All Results.
    float total_Pr_over_Pt;
    if (result.is_los){
        float direct_attenuation =
                result.direct.tx_gain + result.direct.rx_gain - result.direct.direct_loss;
        total_Pr_over_Pt = pow(10, direct_attenuation / 10.0f);
    } else {
        float diffract_attenuation =
                result.diffraction.tx_gain + result.diffraction.rx_gain - result.diffraction.diffraction_loss;
        total_Pr_over_Pt = pow(10, diffract_attenuation / 10.0f);
    }
    for(const auto & reflection : result.reflections){
        const float & tx_gain = reflection.tx_gain;
        const float & rx_gain = reflection.rx_gain;
        const float & ref_loss = reflection.reflection_loss;
        float reflect_attenuation = tx_gain + rx_gain - ref_loss;
        total_Pr_over_Pt += pow(10, reflect_attenuation / 10.0f);
    }
    result.total_attenuation = 10*log10(total_Pr_over_Pt);
    result.total_received_power = result.total_attenuation + result.transmit_power;
	return true;
}

bool RayTracer::CalculatePathLossMap(const glm::vec3 tx_position,  const float tx_frequency,
                                     const glm::vec3 rx_position,
                                     const std::vector<Record> records,
                                     Result &result) const {
    // Validation of Result
    if (records.empty()) {
        result.is_valid = false;
        return false;
    }
    result.is_valid = true;

    for (auto &record: records) {
        switch (record.type) {
            case RecordType::kDirect: {
                result.is_los = true;
                // Calculate Distance
                float distance = glm::distance(tx_position, rx_position);

                // Friis's Equation
                float free_space_loss = 20*log10(distance) + 20*log10(tx_frequency) - 147.55f;
                result.direct.direct_loss = free_space_loss;

                // Calculate delay
                result.direct.delay = distance/LIGHT_SPEED;
            }
                break;
            case RecordType::kReflect: {
                for(auto ref_position : record.data){
                    // Get distances.
                    float d1 = glm::distance(tx_position, ref_position);
                    float d2 = glm::distance(rx_position, ref_position);
                    float total_distance = d1+d2; // total distance

                    constexpr Polarization polar = TE;
                    // Calculate Reflection Coefficient.
                    const float ref_coe = CalculateReflectionCoefficient(tx_position, rx_position,
                                                                         ref_position, polar);
                    // Calculate Receive Power.
                    float reflection_loss = 20*log10(total_distance) + 20*log10(tx_frequency) - 20*log10(abs(ref_coe)) - 147.55f;

                    // Calculate delay.
                    float delay = total_distance/LIGHT_SPEED;

                    // Store the values in result.
                    result.reflections.push_back(ReflectionResult{reflection_loss, delay, 0, 0});

                }

            }
                break;
            case RecordType::kEdgeDiffraction: {
                float distance = glm::distance(tx_position, rx_position);
                float free_space_loss = 20 * log10(distance) + 20 * log10(tx_frequency) - 147.55f;

                if (record.data.size() == 1) {
                    // Get Edge Position from Record.
                    const glm::vec3 edge_position = record.data[0];

                    // Calculate V
                    float v = CalculateVOfEdge(tx_position, edge_position, rx_position, tx_frequency);

                    // Calculate Diffraction Loss from V and FSPL.
                    float diff_loss = free_space_loss + CalculateDiffractionByV(v);

                    // Calculate the receive power
                    result.diffraction.diffraction_loss = diff_loss;

                    // Calculate Delay
                    float distance = glm::distance(edge_position, tx_position) +
                                     glm::distance(edge_position, rx_position);

                    result.diffraction.delay = distance / LIGHT_SPEED;
                }
                else if (record.data.size() == 2) {
                    // Copy Edges to stack.
                    std::vector<glm::vec3> edges = record.data;
                    const glm::vec3 nearest_tx_edge = NearestEdgeFromPoint(tx_position, edges);
                    const glm::vec3 nearest_rx_edge = NearestEdgeFromPoint(rx_position, edges);

                    // Find the max V as single edge V
                    const float v1 = CalculateVOfEdge(tx_position, nearest_tx_edge, rx_position, tx_frequency);
                    const float v2 = CalculateVOfEdge(tx_position, nearest_rx_edge, rx_position, tx_frequency);
                    const float max_v = std::max({v1, v2});
                    float diff_loss = free_space_loss + CalculateDiffractionByV(max_v);

                    // Calculate Receive Power
                    result.diffraction.diffraction_loss = diff_loss;

                    // Calculate Delay
                    float distance = glm::distance(tx_position, nearest_tx_edge) + glm::distance(nearest_tx_edge, nearest_rx_edge)
                                     + glm::distance(nearest_rx_edge, rx_position);
                    result.diffraction.delay = distance / LIGHT_SPEED;
                }
                else if (record.data.size() == 3) {
                    // Copy Edges to find edge positions.
                    std::vector<glm::vec3> edges = record.data;
                    glm::vec3 near_tx_pos = NearestEdgeFromPoint(tx_position, edges);
                    glm::vec3 center_pos = NearestEdgeFromPoint(near_tx_pos, edges);
                    glm::vec3 near_rx_pos = NearestEdgeFromPoint(rx_position, edges);


                    // Calculate C1, C2, and C3.
                    float c1 = CalculateSingleKnifeEdge(tx_position, near_tx_pos, center_pos, tx_frequency);
                    float c2 = CalculateSingleKnifeEdge(tx_position, center_pos, rx_position, tx_frequency);
                    float c3 = CalculateSingleKnifeEdge(center_pos, near_rx_pos, rx_position, tx_frequency);

                    // Calculate Corrections.
                    std::pair<float, float> correction_cosines;
                    CalculateCorrectionCosines(tx_position, record.data, rx_position, correction_cosines);
                    float c_1_cap = (6.0f - c2 + c1) * correction_cosines.first;
                    float c_2_cap = (6.0f - c2 + c3) * correction_cosines.second;

                    // Calculate Diffraction Loss
                    float diffraction_loss = free_space_loss + (c2 + c1 + c3 - c_1_cap - c_2_cap);

                    // Calculate Received Power
                    result.diffraction.diffraction_loss = diffraction_loss;

                    // Calculate Delay
                    float d1 = glm::distance(tx_position, near_tx_pos);
                    float d2 = glm::distance(near_tx_pos, center_pos);
                    float d3 = glm::distance(center_pos, near_rx_pos);
                    float d4 = glm::distance(near_rx_pos, rx_position);
                    float total_distance = d1 + d2 + d3 + d4;
                    result.diffraction.delay = total_distance / LIGHT_SPEED;
                }
                else {
                    // Find highest Vs in the edges
                    std::set<float> highest_v;
                    std::map<float, glm::vec3> edges_dict;
                    for (auto edge : record.data) {
                        float temp_v = CalculateVOfEdge(tx_position, edge, rx_position, tx_frequency);
                        edges_dict[temp_v] = edge;
                        highest_v.insert(temp_v);
                    }
                    std::vector<glm::vec3> edges;
                    int i = 0;
                    // choose the highest v from the highest_v
                    for (auto ritr = highest_v.rbegin(); ritr != highest_v.rend(); ++ritr) {
                        edges.push_back(edges_dict[*ritr]);
                        if (i++ == 2) break;
                    }
                    glm::vec3 near_tx_pos = NearestEdgeFromPoint(tx_position, edges);
                    glm::vec3 center_pos = NearestEdgeFromPoint(near_tx_pos, edges);
                    glm::vec3 near_rx_pos = NearestEdgeFromPoint(rx_position, edges);

                    // Calculate C1, C2, and C3.
                    float c1 = CalculateSingleKnifeEdge(tx_position, near_tx_pos, center_pos, tx_frequency);
                    float c2 = CalculateSingleKnifeEdge(tx_position, center_pos, rx_position, tx_frequency);
                    float c3 = CalculateSingleKnifeEdge(center_pos, near_rx_pos, rx_position, tx_frequency);

                    // Calculate Corrections.
                    std::pair<float, float> correction_cosines;
                    CalculateCorrectionCosines(tx_position, record.data, rx_position, correction_cosines);
                    float c_1_cap = (6.0f - c2 + c1) * correction_cosines.first;
                    float c_2_cap = (6.0f - c2 + c3) * correction_cosines.second;

                    // Calculate Diffraction Loss
                    float diffraction_loss = free_space_loss + (c2 + c1 + c3 - c_1_cap - c_2_cap);

                    // Calculate Received Power
                    result.diffraction.diffraction_loss = diffraction_loss;

                    // Calculate Delay
                    float d1 = glm::distance(tx_position, near_tx_pos);
                    float d2 = glm::distance(near_tx_pos, center_pos);
                    float d3 = glm::distance(center_pos, near_rx_pos);
                    float d4 = glm::distance(near_rx_pos, rx_position);
                    float total_distance = d1 + d2 + d3 + d4;
                    result.diffraction.delay = total_distance / LIGHT_SPEED;
                }
            } break;
        }

    }

    // Summary All Results.
    float total_Pr_over_Pt;
    if (result.is_los){
        float direct_attenuation =
                result.direct.tx_gain + result.direct.rx_gain - result.direct.direct_loss;
        total_Pr_over_Pt = pow(10, direct_attenuation / 10.0f);
    } else {
        float diffract_attenuation =
                result.diffraction.tx_gain + result.diffraction.rx_gain - result.diffraction.diffraction_loss;
        total_Pr_over_Pt = pow(10, diffract_attenuation / 10.0f);
    }
    for(const auto reflection : result.reflections){
        float reflect_attenuation = reflection.tx_gain +
                                    reflection.rx_gain -
                                    reflection.reflection_loss;

        total_Pr_over_Pt += pow(10, reflect_attenuation / 10.0f);
    }
    result.total_attenuation = 10*log10(total_Pr_over_Pt);
    result.total_received_power = result.total_attenuation + result.transmit_power;
    return true;
}

float RayTracer::CalculateSingleEdgeDiffraction(glm::vec3 tx_pos, glm::vec3 edge_pos, glm::vec3 rx_pos, float tx_freq)
{
    float v = CalculateVOfEdge(tx_pos, edge_pos, rx_pos, tx_freq);
    return CalculateDiffractionByV(v);
}

float RayTracer::CalculateDoubleEdgeDiffraction(glm::vec3 tx_pos, glm::vec3 edge1, glm::vec3 edge2, glm::vec3 rx_pos, float tx_freq)
{
    // Scan max v as main edge
    float v1 = CalculateVOfEdge(tx_pos, edge1, rx_pos, tx_freq);
    float v2 = CalculateVOfEdge(tx_pos, edge2, rx_pos, tx_freq);
    float primary_diffraction = 0.0f, secondary_diffraction = 0.0f, secondary_v = 0.0f;
    if (v1 > v2) {
        // edge1 is the primary edge
        primary_diffraction = CalculateDiffractionByV(v1);
        secondary_diffraction = CalculateDiffractionByV(CalculateVOfEdge(edge1, edge2, rx_pos, tx_freq) );
    }
    else {
        // edge2 is the primatry edge
        primary_diffraction = CalculateDiffractionByV(v2);
        secondary_diffraction = CalculateDiffractionByV(CalculateVOfEdge(tx_pos, edge1, edge2, tx_freq));
    }
    return primary_diffraction + secondary_diffraction;
}

float RayTracer::CalculateTripleEdgeDiffraction(glm::vec3 tx_pos, glm::vec3 edge1, glm::vec3 edge2, glm::vec3 edge3, glm::vec3 rx_pos, float tx_freq)
{
    float v1 = CalculateVOfEdge(tx_pos, edge1, rx_pos, tx_freq);
    float v2 = CalculateVOfEdge(tx_pos, edge2, rx_pos, tx_freq);
    float v3 = CalculateVOfEdge(tx_pos, edge3, rx_pos, tx_freq);
    float max_v = std::max({v1, v2, v3});
    float primary_diffraction = 0.0f, secondary1_diffraction = 0.0f, secondary2_diffraction = 0.0f;
    if (max_v == v1) {
        primary_diffraction = CalculateDiffractionByV(v1);
        secondary1_diffraction = CalculateDiffractionByV(CalculateVOfEdge(edge1, edge2, edge3, tx_freq));
        secondary2_diffraction = CalculateDiffractionByV(CalculateVOfEdge(edge2, edge3, rx_pos, tx_freq));
    }
    else if (max_v == v2) {
        primary_diffraction = CalculateDiffractionByV(v2);
        secondary1_diffraction = CalculateDiffractionByV(CalculateVOfEdge(tx_pos, edge1, edge2, tx_freq));
        secondary2_diffraction = CalculateDiffractionByV(CalculateVOfEdge(edge2, edge3, rx_pos, tx_freq));
    }
    else {
        primary_diffraction = CalculateDiffractionByV(v3);
        secondary1_diffraction = CalculateDiffractionByV(CalculateVOfEdge(tx_pos, edge1, edge2, tx_freq));
        secondary1_diffraction = CalculateDiffractionByV(CalculateVOfEdge(edge1, edge3, edge3, tx_freq));
    }
    return 0.0f;
}

bool RayTracer::IsDirectHit(glm::vec3 start_position,glm::vec3 end_position) const
{
	// get direction from start point to end point
	glm::vec3 direction = glm::normalize(end_position - start_position);
	float start_to_end_distance = glm::distance(start_position, end_position);
	Ray ray{ start_position, direction };
	float distance = FLT_MAX;
	// trace the ray on this direction 
	// check if the direction hit something and the t is not between start_point to end_point length
	if (map_->IsHit(ray, distance) && distance < start_to_end_distance)
		return false;

	return true;
}

bool RayTracer::IsReflected(const glm::vec3 start_position, const glm::vec3 end_position, std::vector<glm::vec3>& reflected_points) const
{
	// Match the co-exist triangles between two points
	std::vector<const Triangle*> check_triangles;

	// Searching for check triangles
	/*if (map_->GetObjects().size() > 129600) {
		// scan hit_triangles
		std::map<Triangle *, bool> start_hits = ScanHit(start_position); // it won't scan if the point is already checked
		std::map<Triangle*, bool> end_hits = ScanHit(end_position);
		for (auto const& [triangle, exist_value] : start_hits)
			if (end_hits[triangle]) check_triangles.push_back(triangle);
	}
	else {
		check_triangles = map_->GetObjects();
	}*/
    check_triangles = map_->GetObjects();
	// check the reflections points on matches triangles

	for (const Triangle * matched_triangle : check_triangles) {
		// reflect one of the point on the triangle plane
		glm::vec3 reflected_position = ReflectedPointOnTriangle(matched_triangle, start_position);

		// Trace from the reflected point
		glm::vec3 ref_to_end_direction = glm::normalize(end_position - reflected_position);

		Ray ref_to_end_ray{ reflected_position, ref_to_end_direction };
        // hit triangles from reflected_position to end_position
		std::unordered_map<const Triangle *, float > hit_triangles; 

		if (map_->IsHit(ref_to_end_ray, hit_triangles) && hit_triangles.find(matched_triangle) != hit_triangles.end()) {
			/*for (auto const [triangle, distance] : hit_triangles) {
				if (triangle == matched_triangle) {
					reflection_point_position = reflected_position + ref_to_end_direction * (distance + 0.001f);
					if (IsDirectHit(reflection_point_position, end_position) &&
						IsDirectHit(reflection_point_position, start_position))
						reflected_points.push_back(reflection_point_position);
					break;
				}
			}*/
            float distance = hit_triangles.find(matched_triangle)->second;
            // add small value to move the point to surface.
            glm::vec3 reflection_point_position = reflected_position + ref_to_end_direction * (distance + 0.001f);
            if (IsDirectHit(reflection_point_position, end_position) &&
                IsDirectHit(reflection_point_position, start_position))
                reflected_points.push_back(reflection_point_position);
		}
	}
	if (reflected_points.empty()) return false;
	return true;
}

float RayTracer::CalculateReflectionCoefficient(glm::vec3 start_position, glm::vec3 end_position,
                                                glm::vec3 reflection_position, Polarization polar)
{
    // Directions.
	glm::vec3 ref_to_start_direction = glm::normalize(start_position - reflection_position);
	glm::vec3 ref_to_end_direction = glm::normalize(end_position - reflection_position);

    //Angle between the reflections direction to the normal of surface
    float angle_1 = glm::angle(ref_to_start_direction, ref_to_end_direction)/2.0f;

	// Calculate the angle_2
	constexpr float n1 = AIR_IOR; // air
	constexpr float n2 = CONCRETE_IOR; // concrete
	constexpr float c = LIGHT_SPEED;
	float c1 = c / sqrt(n1);
	float c2 = c / sqrt(n2);

    // Refraction angle according to Snell's law.
	float angle_2 = asin(c2*sin(angle_1)/c1);

	// Calculate depends on Polarization.
	switch (polar) {
	case TE: {
		if (sqrt(abs(n1 / n2)) * sin(angle_1) >= 1) return 1.0f;
		return (sqrt(n1)*cos(angle_1)  - sqrt(n2)*cos(angle_2)) /
				(sqrt(n1)*cos(angle_1) + sqrt(n2)*cos(angle_2));
	}
	case TM: {
		if (sqrt(abs(n1 / n2)) * sin(angle_1) >= 1) return 1.0f;
		return (sqrt(n2) * cos(angle_1) - sqrt(n1) * cos(angle_2)) /
				(sqrt(n2) * cos(angle_1) + sqrt(n1) * cos(angle_2));
	}
	}
	return 1.0f;
}


glm::vec3 RayTracer::ReflectedPointOnTriangle(const Triangle* triangle, glm::vec3 points)
{
	/// The reflections point on the triangle plane can be calculated as following:

	// 1. construct the plane from 3 points (non-collinear points)
	auto triangle_points = triangle->GetPoints();
	const glm::vec3 & p_0 = triangle_points[0];
    const glm::vec3 & p_1 = triangle_points[1];
    const glm::vec3 & p_2 = triangle_points[2];

	glm::vec3 n = triangle->GetNormal();

	float b = (n.x * p_1.x) + (n.y * p_1.y) + (n.z * p_1.z);
	// 2. mirror the point from the plane
	float t = (b - (points.x * n.x + points.y * n.y + points.z * n.z)) / (n.x * n.x + n.y * n.y + n.z * n.z); //distance from point to plane
	
	// (Optional) Point on surface
	// glm::vec3 m = glm::vec3(points.x + t * n.x, points.y + t * n.y, points.z + t * n.z); // points on mirror

	return  points + 2 * t * n; // reverse average point from average point 
}

bool RayTracer::IsKnifeEdgeDiffraction( const glm::vec3 start_position, const glm::vec3 end_position, std::vector<glm::vec3>& edges_points) const
{
	const unsigned int max_scan = 10; // Maximum Scan
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
				CleanEdgePoints(start_position, end_position, edges_points);
				if (edges_points.size() == 0) return false;
				return true;
			}
			edges_points.push_back(edge_from_left_position);
			edges_points.push_back(edge_from_right_position);
			CleanEdgePoints(start_position, end_position, edges_points);
			if (edges_points.size() == 0) return false;
			return true;
		}
		// search more eges
		left_position = edge_from_left_position;
		right_position = edge_from_right_position;
		edges_points.push_back(edge_from_left_position);
		edges_points.push_back(edge_from_right_position);
	}
	return false;
}

bool RayTracer::FindEdge(const glm::vec3 start_position,const glm::vec3 end_position, glm::vec3& edge_position) const
{
	const glm::vec3 up_direction = glm::vec3(0.0f, 1.0f, 0.0f);
	const float scan_precision = .5f;
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
		if (glm::degrees(glm::angle(scan_direction, up_direction)) < 1.0f) return false;

		Ray scan_ray{ start_position, scan_direction }; //implement to heap later

		if (map_->IsHit(scan_ray, scan_hit_distance)) {
			latest_hit_position = start_position + scan_direction * scan_hit_distance;
			// latest_hit_position should be in between start_positon and end_position in xz plane
			if (latest_hit_position.x < min_x || latest_hit_position.x > max_x ||
				latest_hit_position.z < min_z || latest_hit_position.z > max_z) {
				break;
			}
			latest_hit_distance = scan_hit_distance;
			latest_hit_direction = scan_direction;
		}
		else {
			if (latest_hit_distance == -1.0f) return false;
			break;
		}
	}

	glm::vec3 start_end_on_xz_direction = glm::normalize(glm::vec3(start_end_direction.x, 0.0f, start_end_direction.z));
	float start_end_to_on_xz_angle = glm::angle(latest_hit_direction, start_end_on_xz_direction);
	float distance_on_xz = latest_hit_distance * cos(start_end_to_on_xz_angle);
	float start_edge_angle = glm::angle(start_end_on_xz_direction, scan_direction);
	float distance_to_edge = distance_on_xz / cos(start_edge_angle);

	// calculate the start_edge_point after exit the obstacles
	edge_position = start_position + scan_direction * distance_to_edge;
	return true;
}

glm::vec3 RayTracer::NearestEdgeFromPoint(glm::vec3 point_position, std::vector<glm::vec3>& edges_points)
{
	std::map<float, glm::vec3> distance_from_point;
	for (auto edge_point : edges_points) { // implemenet to function
		glm::vec3 point_position_on_xz = glm::vec3(point_position.x, 0.0f, point_position.z);
		glm::vec3 edge_point_on_xz = glm::vec3(edge_point.x, 0.0f, edge_point.z);
		distance_from_point[glm::distance(edge_point_on_xz, point_position_on_xz)] = edge_point;
	}
	edges_points.erase(std::remove(edges_points.begin(), edges_points.end(), distance_from_point.begin()->second), edges_points.end());
	return glm::vec3(distance_from_point.begin()->second);
}

void RayTracer::CleanEdgePoints(const glm::vec3 start_position, const glm::vec3 end_position, std::vector<glm::vec3>& edges_points) const
{
	if (edges_points.size() == 1) return;

	if (edges_points.size() < 3) {
		for (int i = edges_points.size() - 1; i >= 0; --i)
			for (int j = i - 1; j >= 0; --j) {
				glm::vec3 i_edge = edges_points[i];
				glm::vec3& j_edge = edges_points[j];
				if (glm::distance(i_edge, j_edge) <= .5f) {
					j_edge = (i_edge + j_edge) / 2.0f;
					edges_points.pop_back();
					break;
				};
			}
		return;
	}
	// Find the highest point to reference the check point
	std::vector<glm::vec3> left_to_right_edges;
	std::vector<glm::vec3> optimized_edges;
	std::vector<glm::vec3> unoptimized_edges = edges_points;
	left_to_right_edges.push_back(start_position);
	while (!unoptimized_edges.empty())
		left_to_right_edges.push_back(NearestEdgeFromPoint(left_to_right_edges.back(),unoptimized_edges));
	left_to_right_edges.push_back(end_position);
	// Trace the lestest path to furthest path for direct hit.
	for (unsigned int i = 0; i < left_to_right_edges.size()-1; ++i) {
		glm::vec3 stand_edge = left_to_right_edges[i];
		for (unsigned int j = left_to_right_edges.size() - 1; j > i; --j) {
			if (i == j -1) {
				if(stand_edge != start_position) optimized_edges.push_back(stand_edge);
				break;
			}
			glm::vec3 check_edge = left_to_right_edges[j];
			if (IsDirectHit(stand_edge, check_edge)) {
				if(i!=0)optimized_edges.push_back(stand_edge);
				i = j-1;
				break;
			}		
		}
	}
	edges_points = optimized_edges;
	if (edges_points.size() < 2) return;
	for (int i = edges_points.size() - 1; i >= 0; --i)
		for (int j = i - 1; j >= 0; --j) {
			glm::vec3 i_edge = edges_points[i];
			glm::vec3& j_edge = edges_points[j];
			if (glm::distance(i_edge, j_edge) <= .5f) {
				j_edge = (i_edge + j_edge) / 2.0f;
				edges_points.pop_back();
				break;
			};
		}
	return;
}

float RayTracer::GetHighestPoint(std::vector<glm::vec3> edges) const
{
	float highest_point = 0;
	for (auto edge : edges)
		if (edge.y > highest_point) {
			highest_point = edge.y;
		}
	return highest_point;
}

float RayTracer::CalculateSingleKnifeEdge(glm::vec3 start_position, glm::vec3 edge_position, glm::vec3 end_position, float frequency) const
{
	float v = CalculateVOfEdge(start_position, edge_position, end_position, frequency);

	return CalculateDiffractionByV(v);
}

float RayTracer::CalculateDiffractionByV(float v) {
	return 6.9f + 20.0 * log10(sqrt(pow(v - 0.1, 2) + 1) + v - 0.1);
}

float RayTracer::CalculateVOfEdge(glm::vec3 start_position, glm::vec3 edge_position,
                                  glm::vec3 end_position, float frequency) {
	
	float wave_length = 3e8 / (frequency);
	glm::vec3 start_to_end_direction = glm::normalize(end_position - start_position);

	glm::vec3 start_to_edge_direction = glm::normalize(edge_position - start_position);
	glm::vec3 end_to_edge_direction = glm::normalize(edge_position - end_position);

	float angle_1 = glm::angle(start_to_edge_direction, start_to_end_direction);
	float angle_2 = glm::angle(end_to_edge_direction, -start_to_end_direction);
	float r1 = glm::distance(start_position, edge_position);
	float r2 = glm::distance(end_position, edge_position);
	float s1 = r1 * cos(angle_1);
	float s2 = r2 * cos(angle_2);
	float h = sin(angle_1) * r1;
	float v = h * sqrt((2.0f / wave_length) * (s1 + s2) / (r1 * r2));

	return v;
}

void RayTracer::CalculateCorrectionCosines(	glm::vec3 start_position, std::vector<glm::vec3> edges,
											glm::vec3 end_position, std::pair<float, float>& calculated_cosines)
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

	calculated_cosines.first = sqrt((d1 * (d3 + d4)) / ((d1 + d2) * (d2 + d3 + d4)));
	calculated_cosines.second = sqrt((d4 * (d1 + d2)) / ((d3 + d4) * (d1 + d2 + d3)));
	return;
}

void RayTracer::CalculateDirectPath(const Record &record, Result &result, Transmitter *transmitter, Receiver *receiver) const {
    result.is_los = true;
    // Get Transmitter's Info
    auto tx_pos = transmitter->GetPosition();
    auto tx_freq = transmitter->GetFrequency();
    // Get Receiver's Info
    auto rx_pos = receiver->GetPosition();
    // Calculate tx and rx gain.
    auto tx_gain = transmitter->GetTransmitterGain(rx_pos);
    auto rx_gain = receiver->GetReceiverGain(tx_pos);
    result.direct.tx_gain = tx_gain;
    result.direct.rx_gain = rx_gain;
    // Calculate Distance
    float distance = glm::distance(tx_pos, rx_pos);

    // Friis's Equation
    float free_space_loss = 20*log10(distance) + 20*log10(tx_freq) - 147.55f;
    result.direct.direct_loss = free_space_loss;

    // Calculate delay
    result.direct.delay = distance/LIGHT_SPEED;
}

void RayTracer::CalculateReflections(const Record &record, Result &result, Transmitter *transmitter, Receiver *receiver) const {
    // Get Transmitter's Info.
    const auto & tx_pos = transmitter->GetPosition();
    const auto & tx_freq = transmitter->GetFrequency();
    const auto & tx_power = transmitter->GetTransmitPower();

    // Get Receiver's Info
    const auto & rx_pos = receiver->GetPosition();

    // Prepare Threads
    std::vector<std::thread> threads;
    for (auto & reflect_position : record.data) {
        // Get gains before compute.
        float tx_gain = transmitter->GetTransmitterGain(reflect_position);
        float rx_gain = receiver->GetReceiverGain(reflect_position);
        // construct a thread.
        std::thread reflection_thread(&RayTracer::CalculateReflection, this,
                                      tx_pos, rx_pos, tx_freq, tx_gain,
                                      rx_gain, tx_power, reflect_position, std::ref(result));
        threads.push_back(std::move(reflection_thread));
    }

    // Run Threads
    for (std::thread & thread: threads)
        if (thread.joinable()) thread.join();
}

void RayTracer::CalculateDiffraction(const Record &record, Result &result, Transmitter *transmitter, Receiver *receiver) const {
    result.is_los = false;
    // Get Transmitter's Info
    const auto &tx_pos = transmitter->GetPosition();
    const auto &tx_freq = transmitter->GetFrequency();
    const auto &tx_power = transmitter->GetTransmitPower();
    // Get Receiver's Info
    const auto &rx_pos = receiver->GetPosition();

    // Calculate Free Space Path Loss
    const float distance = glm::distance(tx_pos, rx_pos);
    float free_space_loss = 20 * log10(distance) + 20 * log10(tx_freq) - 147.55f;

    if (record.data.size() == 1) {
        // Get Edge Position from Record.
        const glm::vec3 edge_position = record.data[0];

        // Calculate the gain.
        const auto &tx_gain = transmitter->GetTransmitterGain(edge_position);
        const auto &rx_gain = receiver->GetReceiverGain(edge_position);
        result.diffraction.tx_gain = tx_gain;
        result.diffraction.rx_gain = rx_gain;

        // Calculate V
        float v = CalculateVOfEdge(tx_pos, edge_position, rx_pos, tx_freq);

        // Calculate Diffraction Loss from V and FSPL.
        float diff_loss = free_space_loss + CalculateDiffractionByV(v);

        // Calculate the receive power
        result.diffraction.diffraction_loss = diff_loss;

        // Calculate Delay
        float distance = glm::distance(edge_position, tx_pos) +
                         glm::distance(edge_position, rx_pos);

        result.diffraction.delay = distance / LIGHT_SPEED;
    } 
    else if (record.data.size() == 2) {
        // Copy Edges to stack.
        std::vector<glm::vec3> edges = record.data;
        const glm::vec3 nearest_tx_edge = NearestEdgeFromPoint(tx_pos, edges);
        const glm::vec3 nearest_rx_edge = NearestEdgeFromPoint(rx_pos, edges);
        // Calculate tx, rx gains
        const float &tx_gain = transmitter->GetTransmitterGain(nearest_tx_edge);
        const float &rx_gain = receiver->GetReceiverGain(nearest_rx_edge);
        result.diffraction.tx_gain = tx_gain;
        result.diffraction.rx_gain = rx_gain;
        // Find the max V as single edge V
        glm::vec3 edge_posiiton;
        const float v1 = CalculateVOfEdge(tx_pos, nearest_tx_edge, rx_pos, tx_freq);
        const float v2 = CalculateVOfEdge(tx_pos, nearest_rx_edge, rx_pos, tx_freq);
        const float max_v = std::max({v1, v2});
        float diff_loss = free_space_loss + CalculateDiffractionByV(max_v);

        // Calculate Receive Power
        result.diffraction.diffraction_loss = diff_loss;

        // Calculate Delay
        float distance = glm::distance(tx_pos, nearest_tx_edge) + glm::distance(nearest_tx_edge, nearest_rx_edge)
                         + glm::distance(nearest_rx_edge, rx_pos);
        result.diffraction.delay = distance / LIGHT_SPEED;
    } 
    else if (record.data.size() == 3) {
        // Copy Edges to find edge positions.
        std::vector<glm::vec3> edges = record.data;
        glm::vec3 near_tx_pos = NearestEdgeFromPoint(tx_pos, edges);
        glm::vec3 center_pos = NearestEdgeFromPoint(near_tx_pos, edges);
        glm::vec3 near_rx_pos = NearestEdgeFromPoint(rx_pos, edges);

        // Calculate tx and rx gain.
        const float &tx_gain = transmitter->GetTransmitterGain(near_tx_pos);
        const float &rx_gain = receiver->GetReceiverGain(near_rx_pos);

        result.diffraction.tx_gain = tx_gain;
        result.diffraction.rx_gain = rx_gain;

        // Calculate C1, C2, and C3.
        float c1 = CalculateSingleKnifeEdge(tx_pos, near_tx_pos, rx_pos, tx_freq);
        float c2 = CalculateSingleKnifeEdge(tx_pos, center_pos, rx_pos, tx_freq);
        float c3 = CalculateSingleKnifeEdge(tx_pos, near_rx_pos, rx_pos, tx_freq);

        // Calculate Corrections.
        std::pair<float, float> correction_cosines;
        CalculateCorrectionCosines(tx_pos, record.data, rx_pos, correction_cosines);
        float c_1_cap = (6.0f - c2 + c1) * correction_cosines.first;
        float c_2_cap = (6.0f - c2 + c3) * correction_cosines.second;

        // Calculate Diffraction Loss
        float diffraction_loss = free_space_loss + (c2 + c1 + c3 - c_1_cap - c_2_cap);

        // Calculate Received Power
        result.diffraction.diffraction_loss = diffraction_loss;

        // Calculate Delay
        float d1 = glm::distance(tx_pos, near_tx_pos);
        float d2 = glm::distance(near_tx_pos, center_pos);
        float d3 = glm::distance(center_pos, near_rx_pos);
        float d4 = glm::distance(near_rx_pos, rx_pos);
        float total_distance = d1 + d2 + d3 + d4;
        result.diffraction.delay = total_distance / LIGHT_SPEED;
    } 
    else {
        // Find highest Vs in the edges
        std::set<float> highest_v;
        std::map<float, glm::vec3> edges_dict;
        for (auto edge : record.data) {
            float temp_v = CalculateVOfEdge(tx_pos, edge, rx_pos, tx_freq);
            edges_dict[temp_v] = edge;
            highest_v.insert(temp_v);
        }
        std::vector<glm::vec3> edges;
        int i = 0;
        // choose the highest v from the highest_v
        for (auto ritr = highest_v.rbegin(); ritr != highest_v.rend(); ++ritr) {
            edges.push_back(edges_dict[*ritr]);
            if (i++ == 2) break;
        }
        glm::vec3 near_tx_pos = NearestEdgeFromPoint(tx_pos, edges);
        glm::vec3 center_pos = NearestEdgeFromPoint(near_tx_pos, edges);
        glm::vec3 near_rx_pos = NearestEdgeFromPoint(rx_pos, edges);

        // Calculate tx and rx gain.
        const float &tx_gain = transmitter->GetTransmitterGain(near_tx_pos);
        const float &rx_gain = receiver->GetReceiverGain(near_rx_pos);
        result.diffraction.tx_gain = tx_gain;
        result.diffraction.rx_gain = rx_gain;


        // Calculate C1, C2, and C3.
        float c1 = CalculateSingleKnifeEdge(tx_pos, near_tx_pos, center_pos, tx_freq);
        float c2 = CalculateSingleKnifeEdge(tx_pos, center_pos, rx_pos, tx_freq);
        float c3 = CalculateSingleKnifeEdge(center_pos, near_rx_pos, rx_pos, tx_freq);

        // Calculate Corrections.
        std::pair<float, float> correction_cosines;
        CalculateCorrectionCosines(tx_pos, record.data, rx_pos, correction_cosines);
        float c_1_cap = (6.0f - c2 + c1) * correction_cosines.first;
        float c_2_cap = (6.0f - c2 + c3) * correction_cosines.second;

        // Calculate Diffraction Loss
        float diffraction_loss = free_space_loss + (c2 + c1 + c3 - c_1_cap - c_2_cap);

        // Calculate Received Power
        result.diffraction.diffraction_loss = diffraction_loss;

        // Calculate Delay
        float d1 = glm::distance(tx_pos, near_tx_pos);
        float d2 = glm::distance(near_tx_pos, center_pos);
        float d3 = glm::distance(center_pos, near_rx_pos);
        float d4 = glm::distance(near_rx_pos, rx_pos);
        float total_distance = d1 + d2 + d3 + d4;
        result.diffraction.delay = total_distance / LIGHT_SPEED;
    }
}

void RayTracer::CalculateReflection( const glm::vec3 & tx_position, const glm::vec3 & rx_position,
                                     const float & tx_freq, const float & tx_gain,
                                     const float & rx_gain, const float & tx_power,
                                     const glm::vec3 & ref_position, Result & result) const {

    // Get distances.
    float d1 = glm::distance(tx_position, ref_position);
    float d2 = glm::distance(rx_position, ref_position);
    float total_distance = d1+d2; // total distance

    constexpr Polarization polar = TE;
    // Calculate Reflection Coefficient.
    const float ref_coe = CalculateReflectionCoefficient(tx_position, rx_position,
                                                        ref_position, polar);
    // Calculate Receive Power.
    float reflection_loss = 20*log10(total_distance) + 20*log10(tx_freq) - 20*log10(abs(ref_coe)) - 147.55f;

    // Calculate delay.
    float delay = total_distance/LIGHT_SPEED;

    // Store the values in result.
    result.reflections.push_back(ReflectionResult{reflection_loss, delay, tx_gain, rx_gain});
}

void RayTracer::GetMapBorder(float &min_x, float &max_x, float & min_z, float & max_z) const {
    map_->GetBorders(min_x, max_x, min_z, max_z);
}