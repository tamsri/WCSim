#include "ray_tracer.hpp"

#include <iostream>
#include <set>
#include <utility>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <stack>
#include <queue>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "object.hpp"
#include "cube.hpp"
#include "ray.hpp"
#include "line.hpp"
#include "triangle.hpp"
#include "polygon_mesh.hpp"
#include "shader.hpp"

#include "transmitter.hpp"
#include "receiver.hpp"

#include "transform.hpp"


RayTracer::RayTracer(PolygonMesh* map) :map_(map)
{
}

std::map <Triangle *, bool> RayTracer::ScanHit(const glm::vec3 position) const
{
	std::map<Triangle*, bool> hit_triangles;
	// Approach I: when the triangles are more than the generated scanning rays
	glm::vec4 direction = { 1.0f , 0.0f, 0.0f, 1.0f }; // initial scan direction
	float scan_precision = 1.0f;
	for (float i = 0; i < 360; i += scan_precision)
		for (float j = 0; j < 360; j += scan_precision) {
			auto trans_direction = glm::rotate(glm::mat4(1.0f), glm::radians(i), glm::vec3(0.0f, 1.0f, 0.0f));
			trans_direction = glm::rotate(trans_direction, glm::radians(j), glm::vec3(0.0f, 0.0f, 1.0f));
			auto new_direction = trans_direction * direction;
			glm::vec3 i_direction = glm::vec3(new_direction);

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
	for (float i = 0; i < 360; i += scan_precision)
		for (float j = 0; j < 360; j += scan_precision) {
			auto trans_direction = glm::rotate(glm::mat4(1.0f), glm::radians(i), glm::vec3(0.0f, 1.0f, 0.0f));
			trans_direction = glm::rotate(trans_direction, glm::radians(j), glm::vec3(0.0f, 0.0f, 1.0f));
			auto new_direction = trans_direction * direction;
			glm::vec3 i_direction = glm::vec3(new_direction);

			Ray ray{ position, i_direction };
			Triangle* hit_triangle = nullptr;
			float hit_distance; // doesnt do anything yet // maybe implement later. 
			if (map_->IsHit(ray, hit_distance, hit_triangle)) {
				hit_triangles.push_back(hit_triangle);
			}
		}
	return hit_triangles;
}
void RayTracer::Trace(const glm::vec3 start_position,const glm::vec3 end_position, std::vector<Record> & records) const
{
	// Trace Line of Sight
	using namespace std::chrono;
	auto direct_start = high_resolution_clock::now();
	bool is_direct = IsDirectHit(start_position, end_position);
	auto direct_end = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(direct_end - direct_start);
	//std::cout << "Direct Hit Tracer takes: " << duration.count()/1000.0f << " ms\n";

	if (IsDirectHit(start_position, end_position)) {
		records.push_back(Record{ RecordType::kDirect });
	}
	else {
		// Implement multiple knife-edge diffraction
		std::vector<glm::vec3> edges_points;
		auto diff_start = high_resolution_clock::now();
		if (IsKnifeEdgeDiffraction(start_position, end_position, edges_points)) {
			Record saving_record{ RecordType::kEdgeDiffraction, edges_points };
			records.push_back(saving_record);
		}
		auto diff_end = high_resolution_clock::now();
		auto diff_duration = duration_cast<microseconds>(diff_end - diff_start);
		//std::cout << "Diffraction Tracer takes " << diff_duration.count()/1000.0f << " ms\n";
	}

	// find possible reflections
	std::vector <glm::vec3> reflected_points;
	auto ref_start = high_resolution_clock::now();
	if (IsReflected(start_position, end_position, reflected_points)) {
		//std::cout << "Reflected!!" << std::endl;
		Record saving_record{ RecordType::kReflect, reflected_points };
		records.push_back(saving_record);
	}
	auto ref_end = high_resolution_clock::now();
	auto ref_duration = duration_cast<microseconds>(ref_end - ref_start);
	//std::cout << "Reflection Tracer takes " << ref_duration.count()/1000.0f << " ms\n";
}

void RayTracer::Trace(Transmitter * transmitter, const glm::vec3 end_position, std::vector<Record>& records) const
{
	// Trace Line of Sight
	glm::vec3 start_position = transmitter->GetPosition();
	using namespace std::chrono;
	auto direct_start = high_resolution_clock::now();
	bool is_direct = IsDirectHit(start_position, end_position);
	auto direct_end = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(direct_end - direct_start);
	//std::cout << "Direct Hit Tracer takes: " << duration.count() / 1000.0f << " ms\n";

	if (IsDirectHit(start_position, end_position)) {
		records.push_back(Record{ RecordType::kDirect });
	}
	else {
		// Implement multiple knife-edge diffraction
		std::vector<glm::vec3> edges_points;
		auto diff_start = high_resolution_clock::now();
		if (IsKnifeEdgeDiffraction(start_position, end_position, edges_points)) {
			Record saving_record{ RecordType::kEdgeDiffraction, edges_points };
			records.push_back(saving_record);
		}
		auto diff_end = high_resolution_clock::now();
		auto diff_duration = duration_cast<microseconds>(diff_end - diff_start);
		std::cout << "Diffraction Tracer takes " << diff_duration.count() / 1000.0f << " ms\n";
	}

	// find possible reflections
	std::vector <glm::vec3> reflected_points;
	auto ref_start = high_resolution_clock::now();
	if (IsReflected(transmitter, end_position, reflected_points)) {
		//std::cout << "Reflected!!" << std::endl;
		Record saving_record{ RecordType::kReflect, reflected_points };
		records.push_back(saving_record);
	}
	auto ref_end = high_resolution_clock::now();
	auto ref_duration = duration_cast<microseconds>(ref_end - ref_start);
	std::cout << "Reflection Tracer takes " << ref_duration.count() / 1000.0f << " ms\n";
}

void RayTracer::GetDrawComponents(const glm::vec3 start_position, const glm::vec3 end_position, std::vector<Record>& records, std::vector<Object*>& objects) const
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

bool RayTracer::CalculatePathLoss(const glm::vec3 transmitter_position, const glm::vec3 receiver_position, const float frequency, const std::vector<Record>& records, Result& result) const
{

	if (records.size() == 0)
	{
		result.is_valid = false;
		return false;
	}
	result.is_valid = true;
	const float c = 3e8;
	const float wave_length = c / frequency;

	const float pi = atan(1.0f) * 4;

	result.direct_path_loss_in_linear = 0.0f;
	result.reflection_loss_in_linear = 0.0f;
	result.diffraction_loss_in_linear = 0.0f;

	for (auto record : records) {
		switch (record.type) {
		case RecordType::kDirect: {

			const float distance = glm::distance(transmitter_position, receiver_position);

			result.direct_path_loss_in_linear = 1  / pow(4 * pi * distance / wave_length, 2);
			break;
		}
		case RecordType::kReflect: {

			for (auto reflect_position : record.data) {
				const float ref_coe = CalculateReflectionCofficient(transmitter_position, receiver_position, reflect_position);
				float d1 = glm::distance(reflect_position, transmitter_position);
				float d2 = glm::distance(reflect_position, receiver_position);
				//std::cout << "ref coe: " << ref_coe << std::endl;
				result.reflection_loss_in_linear += 0.8 / (pow(4 * pi * (d1 + d2) / (wave_length * ref_coe), 2));
			}

			break;
		}
		case RecordType::kEdgeDiffraction: {
			const float distance = glm::distance(transmitter_position, receiver_position);
			float free_path_loss_in_dB = 10.0f * log10(pow(4.0f * pi * distance * frequency / c, 2));
			if (record.data.size() == 1) {
				const glm::vec3 edge_position = record.data[0];
				// Single Edge Diffraction Calculation

				float diffraction_loss_in_dB = CalculateSingleKnifeEdge(transmitter_position, edge_position, receiver_position, frequency)
					+ free_path_loss_in_dB ;

				result.diffraction_loss_in_linear = pow(10, -diffraction_loss_in_dB / 10.0f);
			}
			else if (record.data.size() == 2) {
				// Double Edge Diffraction Calculation
				const glm::vec3 edge_1_position = record.data[0];
				const glm::vec3 edge_2_position = record.data[1];
				std::vector<glm::vec3> edges = record.data;
				const glm::vec3 nearest_tx_edge = NearestEdgeFromPoint(transmitter_position, edges);

				float max_v = std::max(
					CalculateVOfEdge(transmitter_position, edge_1_position, receiver_position, frequency),
					CalculateVOfEdge(transmitter_position, edge_2_position, receiver_position, frequency));

				float diffraction_loss_in_dB = CalculateDiffractionByV(max_v) + free_path_loss_in_dB;

				result.diffraction_loss_in_linear = pow(10, -diffraction_loss_in_dB / 10.0f);
			}
			else if (record.data.size() == 3) {
				std::vector<glm::vec3> edges = record.data;
				glm::vec3 near_tramsitter_edge_position = NearestEdgeFromPoint(transmitter_position, edges);
				glm::vec3 center_edge_position = NearestEdgeFromPoint(near_tramsitter_edge_position, edges);
				glm::vec3 near_receiver_edge_position = NearestEdgeFromPoint(receiver_position, edges);

				float c1 = CalculateSingleKnifeEdge(transmitter_position, near_tramsitter_edge_position, receiver_position, frequency);
				float c2 = CalculateSingleKnifeEdge(transmitter_position, center_edge_position, receiver_position, frequency);
				float c3 = CalculateSingleKnifeEdge(transmitter_position, near_receiver_edge_position, receiver_position, frequency);

				std::pair<float, float> correction_cosines;
				CalculateCorrectionCosines(transmitter_position, record.data, receiver_position, correction_cosines);
				float c_1_cap = (6 - c2 + c1) * correction_cosines.first;
				float c_2_cap = (6 - c2 + c3) * correction_cosines.second;
				float diffraction_loss_in_dB = (c2 + c1 + c3 - c_1_cap - c_2_cap);

				diffraction_loss_in_dB = diffraction_loss_in_dB + free_path_loss_in_dB;

				result.diffraction_loss_in_linear = pow(10, -diffraction_loss_in_dB / 10.0f);
			}
			else {
				std::set<float> highest_v;
				std::map<float, glm::vec3> edges_dict;
				for (auto edge : record.data) {
					float temp_v = CalculateVOfEdge(transmitter_position, edge, receiver_position, frequency);
					edges_dict[temp_v] = edge;
					highest_v.insert(temp_v);
				}
				std::vector<glm::vec3> edges; int i = 0;
				// choose the highest v from the highest_v
				for (auto ritr = highest_v.rbegin(); ritr != highest_v.rend(); ++ritr) {
					edges.push_back(edges_dict[*ritr]);
					if (i++ == 2) break;
				}

				glm::vec3 near_tramsitter_edge_position = NearestEdgeFromPoint(transmitter_position, edges);
				glm::vec3 center_edge_position = NearestEdgeFromPoint(near_tramsitter_edge_position, edges);
				glm::vec3 near_receiver_edge_position = NearestEdgeFromPoint(receiver_position, edges);

				float c1 = CalculateSingleKnifeEdge(transmitter_position, near_tramsitter_edge_position, receiver_position, frequency);
				float c2 = CalculateSingleKnifeEdge(transmitter_position, center_edge_position, receiver_position, frequency);
				float c3 = CalculateSingleKnifeEdge(transmitter_position, near_receiver_edge_position, receiver_position, frequency);

				std::pair<float, float> correction_cosines;
				CalculateCorrectionCosines(transmitter_position, record.data, receiver_position, correction_cosines);
				float c_1_cap = (6 - c2 + c1) * correction_cosines.first;
				float c_2_cap = (6 - c2 + c3) * correction_cosines.second;
				float diffraction_loss_in_dB = (c2 + c1 + c3 - c_1_cap - c_2_cap);

				diffraction_loss_in_dB = diffraction_loss_in_dB + free_path_loss_in_dB;

				result.diffraction_loss_in_linear = pow(10, -diffraction_loss_in_dB / 10.0f);

			}
			break;

		}
		}
	}

	// Summarize Calculation
	result.total_loss = 10.0f * log10(result.direct_path_loss_in_linear + result.diffraction_loss_in_linear + result.reflection_loss_in_linear);
	return true;
}

bool RayTracer::CalculatePathLossWithGain(Transmitter * transmitter, const glm::vec3 receiver_position, const std::vector<Record>& records, Result& result) const
{
	if (records.size() == 0) { 
		result.is_valid = false;
		return false; 
	}
	result.is_valid = true;
	const glm::vec3 transmitter_position = transmitter->GetPosition();
	const float frequency = transmitter->GetFrequency();

	const float c = 3e8;
	const float wave_length = c / frequency;

	const float pi = atan(1.0f) * 4;

	result.direct_path_loss_in_linear = 0.0f;
	result.reflection_loss_in_linear = 0.0f;
	result.diffraction_loss_in_linear = 0.0f;

	for (auto record : records) {
		switch (record.type) {
		case RecordType::kDirect: {

			const float tx_gain_in_dB = transmitter->GetTransmitterGain(receiver_position);
			const float tx_gain_in_linear = pow(10, tx_gain_in_dB / 10.0f);

			const float distance = glm::distance(transmitter_position, receiver_position);

			result.direct_path_loss_in_linear = tx_gain_in_linear / pow(4 * pi * distance / wave_length, 2);
			break;
		}
		case RecordType::kReflect: {

			for (auto reflect_position : record.data) {

				const float tx_gain_in_dB = transmitter->GetTransmitterGain(reflect_position);
				const float tx_gain_in_linear = pow(10, tx_gain_in_dB / 10.0f);
				const float ref_coe = CalculateReflectionCofficient(transmitter_position, receiver_position, reflect_position);
				float d1 = glm::distance(reflect_position, transmitter_position);
				float d2 = glm::distance(reflect_position, receiver_position);
				std::cout << "ref coe: " << ref_coe << std::endl;
				result.reflection_loss_in_linear += 0.8 * tx_gain_in_linear / (pow(4 * pi * (d1 + d2) / (wave_length * ref_coe), 2));
			}

			break;
		}
		case RecordType::kEdgeDiffraction: {
			const float distance = glm::distance(transmitter_position, receiver_position);
			float free_path_loss_in_dB = 10.0f * log10(pow(4.0f * pi * distance * frequency / c, 2));
			if (record.data.size() == 1) {
				const glm::vec3 edge_position = record.data[0];
				// Single Edge Diffraction Calculation
				const float tx_gain_in_dB = transmitter->GetTransmitterGain(edge_position);


				float diffraction_loss_in_dB = CalculateSingleKnifeEdge(transmitter_position, edge_position, receiver_position, frequency)
					+ free_path_loss_in_dB - tx_gain_in_dB;

				result.diffraction_loss_in_linear = pow(10, -diffraction_loss_in_dB / 10.0f);
			}
			else if (record.data.size() == 2) {
				// Double Edge Diffraction Calculation
				const glm::vec3 edge_1_position = record.data[0];
				const glm::vec3 edge_2_position = record.data[1];
				std::vector<glm::vec3> edges = record.data;
				const glm::vec3 nearest_tx_edge = NearestEdgeFromPoint(transmitter_position, edges);
				const float tx_gain_in_dB = transmitter->GetTransmitterGain(nearest_tx_edge);

				float max_v = std::max(
					CalculateVOfEdge(transmitter_position, edge_1_position, receiver_position, frequency),
					CalculateVOfEdge(transmitter_position, edge_2_position, receiver_position, frequency));

				float diffraction_loss_in_dB = CalculateDiffractionByV(max_v) + free_path_loss_in_dB - tx_gain_in_dB;

				result.diffraction_loss_in_linear = pow(10, -diffraction_loss_in_dB / 10.0f);
			}
			else if (record.data.size() == 3) {
				std::vector<glm::vec3> edges = record.data;
				glm::vec3 near_tramsitter_edge_position = NearestEdgeFromPoint(transmitter_position, edges);
				glm::vec3 center_edge_position = NearestEdgeFromPoint(near_tramsitter_edge_position, edges);
				glm::vec3 near_receiver_edge_position = NearestEdgeFromPoint(receiver_position, edges);

				const float tx_gain_in_dB = transmitter->GetTransmitterGain(near_tramsitter_edge_position);

				float c1 = CalculateSingleKnifeEdge(transmitter_position, near_tramsitter_edge_position, receiver_position, frequency);
				float c2 = CalculateSingleKnifeEdge(transmitter_position, center_edge_position, receiver_position, frequency);
				float c3 = CalculateSingleKnifeEdge(transmitter_position, near_receiver_edge_position, receiver_position, frequency);

				std::pair<float, float> correction_cosines;
				CalculateCorrectionCosines(transmitter_position, record.data, receiver_position, correction_cosines);
				float c_1_cap = (6.0f - c2 + c1) * correction_cosines.first;
				float c_2_cap = (6.0f - c2 + c3) * correction_cosines.second;
				std::cout << "c1: " << c1 << " c2: " << c2 << " c3: " << c3 << std::endl;
				std::cout << "c_1_cap: " << c_1_cap << ", c_2_cap: " << c_1_cap << std::endl;
 				float diffraction_loss_in_dB = (c2 + c1 + c3 - c_1_cap - c_2_cap);

				diffraction_loss_in_dB = diffraction_loss_in_dB + free_path_loss_in_dB - tx_gain_in_dB;

				result.diffraction_loss_in_linear = pow(10, -diffraction_loss_in_dB / 10.0f);
			}
			else {
				std::set<float> highest_v;
				std::map<float, glm::vec3> edges_dict;
				for (auto edge : record.data) {
					float temp_v = CalculateVOfEdge(transmitter_position, edge, receiver_position, frequency);
					edges_dict[temp_v] = edge;
					highest_v.insert(temp_v);
				}
				std::vector<glm::vec3> edges; int i = 0;
				// choose the highest v from the highest_v
				for (auto ritr = highest_v.rbegin(); ritr != highest_v.rend(); ++ritr) {
					edges.push_back(edges_dict[*ritr]);
					if (i++ == 2) break;
				}

				glm::vec3 near_tramsitter_edge_position = NearestEdgeFromPoint(transmitter_position, edges);
				glm::vec3 center_edge_position = NearestEdgeFromPoint(near_tramsitter_edge_position, edges);
				glm::vec3 near_receiver_edge_position = NearestEdgeFromPoint(receiver_position, edges);

				const float tx_gain_in_dB = transmitter->GetTransmitterGain(near_tramsitter_edge_position);

				float c1 = CalculateSingleKnifeEdge(transmitter_position, near_tramsitter_edge_position, receiver_position, frequency);
				float c2 = CalculateSingleKnifeEdge(transmitter_position, center_edge_position, receiver_position, frequency);
				float c3 = CalculateSingleKnifeEdge(transmitter_position, near_receiver_edge_position, receiver_position, frequency);

				std::pair<float, float> correction_cosines;
				CalculateCorrectionCosines(transmitter_position, record.data, receiver_position, correction_cosines);
				float c_1_cap = (6 - c2 + c1) * correction_cosines.first;
				float c_2_cap = (6 - c2 + c3) * correction_cosines.second;
				float diffraction_loss_in_dB = (c2 + c1 + c3 - c_1_cap - c_2_cap);

				diffraction_loss_in_dB = diffraction_loss_in_dB + free_path_loss_in_dB - tx_gain_in_dB;

				result.diffraction_loss_in_linear = pow(10, -diffraction_loss_in_dB / 10.0f);

			}
			break;

		}
		}
	}

	// Summarize Calculation
	result.total_loss = 10.0f * log10(result.direct_path_loss_in_linear + result.diffraction_loss_in_linear + result.reflection_loss_in_linear);
	return true;
}


bool RayTracer::IsDirectHit(const glm::vec3 start_position,const glm::vec3 end_position) const
{
	// get direction from start point to end point
	glm::vec3 direction = glm::normalize(end_position - start_position);
	float start_to_end_distance = glm::distance(start_position, end_position);
	Ray ray{ start_position, direction };
	float distance = -1;
	// trace the ray on this direction 
	// check if the direction hit something and the t is not betwen start_point to end_point length
	if (map_->IsHit(ray, distance) && distance < start_to_end_distance) {
		return false;
	}
	return true;
}

bool RayTracer::IsReflected(const glm::vec3 start_position, const glm::vec3 end_position, std::vector<glm::vec3>& reflected_points) const
{
	// match the co-exist triangles between two points
	std::vector<const Triangle*> check_triangles;

	// Searching for check triangles
	if (map_->GetObjects().size() > 129600) {
		// scan hit_triangles
		std::map<Triangle *, bool> start_hits = ScanHit(start_position); // it won't scan if the point is already checked
		std::map<Triangle*, bool> end_hits = ScanHit(end_position);
		for (auto const& [triangle, exist_value] : start_hits)
			if (end_hits[triangle] == true) check_triangles.push_back(triangle);
	}
	else {
		check_triangles = map_->GetObjects();
	}

	// check the reflection points on matches triangles

	for (const Triangle* matched_triangle : check_triangles) {
		// reflect one of the point on the triangle plane
		glm::vec3 reflected_position = ReflectedPointOnTriangle(matched_triangle, start_position);
		// trace from the reflected point 
		glm::vec3 ref_to_end_direction = glm::normalize(end_position - reflected_position);

		Ray ref_to_end_ray{ reflected_position, ref_to_end_direction };
		std::set<std::pair<float, Triangle*>> hit_triangles; // hit triangles from reflected_position to end_position

		glm::vec3 reflection_point_position;
		if (map_->IsHit(ref_to_end_ray, hit_triangles)) {

			for (auto const [distance, triangle] : hit_triangles) {
				if (triangle == matched_triangle) {
					reflection_point_position = reflected_position + ref_to_end_direction * (distance + 0.001f);
					if (IsDirectHit(reflection_point_position, end_position) &&
						IsDirectHit(reflection_point_position, start_position))
						reflected_points.push_back(reflection_point_position);
					break;
				}
			}
		}
	}
	if (reflected_points.empty()) return false;
	return true;
}

bool RayTracer::IsReflected( Transmitter * transmitter, const glm::vec3 end_position, std::vector<glm::vec3>& reflected_points) const
{
	// match the co-exist triangles between two points

	glm::vec3 start_position = transmitter->GetPosition();
	if (transmitter->hit_triangles.empty()) transmitter->hit_triangles = ScanHitVec(start_position);
	
	//std::vector<Triangle*> & check_triangles = transmitter->hit_triangles;

	// check the reflection points on matches triangles

	for (const Triangle* matched_triangle : transmitter->hit_triangles) { // why with scan triangle is much slower???
		// reflect one of the point on the triangle plane
		glm::vec3 reflected_position = ReflectedPointOnTriangle(matched_triangle, start_position);
		// trace from the reflected point 
		glm::vec3 ref_to_end_direction = glm::normalize(end_position - reflected_position);

		Ray ref_to_end_ray{ reflected_position, ref_to_end_direction };
		std::set<std::pair<float, Triangle*>> hit_triangles; // hit triangles from reflected_position to end_position


		if (map_->IsHit(ref_to_end_ray, hit_triangles)) {

			for (auto const [distance, triangle] : hit_triangles) {
				if (triangle == matched_triangle) {
					glm::vec3 reflection_point_position = reflected_position + ref_to_end_direction * (distance + 0.001f);
					if (IsDirectHit(reflection_point_position, end_position) &&
						IsDirectHit(reflection_point_position, start_position))
						reflected_points.push_back(reflection_point_position);

					break;
				}
			}

		}
	}
	if (reflected_points.empty()) return false;
	return true;
}

float RayTracer::CalculateReflectionCofficient(glm::vec3 start_position, glm::vec3 end_position, glm::vec3 reflection_position) const
{
	glm::vec3 ref_to_start_direction = glm::normalize(start_position - reflection_position);
	glm::vec3 ref_to_end_direction = glm::normalize(end_position - reflection_position);
	float angle_1 = glm::angle(ref_to_start_direction, ref_to_end_direction)/2.0f; //Angle between the reflection direction to the normal of surface
	enum Polarization : bool {
		TM = true,
		TE = false
	};
	Polarization polar = Polarization::TM; /// Question: TM or TE???

	// Calculate the angle_2
	float n1 = 1.0003f; // air
	float n2 = 5.31f; // concrete's relative permittivity according to ITU-R, P.2040-1. (Only for 1-100 GHz)
	const float c = 3e8;
	float c1 = c / sqrt(n1);
	float c2 = c / sqrt(n2);
	float angle_2 = asin(c2*sin(angle_1)/c1); // Refraction angle according to Snell's law


	std::cout << "angle_1: " << glm::degrees(angle_1) << "deg \n";
	//std::cout << "angle_2: " << glm::degrees(angle_2) << "deg \n";

	switch (polar) {
	case TE: {
		if (sqrt(abs(n1 / n2)) * sin(angle_1) >= 1) return 1.0f;
		return (sqrt(n1)*cos(angle_1)  - sqrt(n2)*cos(angle_2)) / (sqrt(n1)*cos(angle_1) + sqrt(n2)*cos(angle_2));
	}
	case TM: {
		if (sqrt(abs(n1 / n2)) * sin(angle_1) >= 1) return 1.0f;
		return (sqrt(n2) * cos(angle_1) - sqrt(n1) * cos(angle_2)) / (sqrt(n2) * cos(angle_1) + sqrt(n1) * cos(angle_2));
	}
	}
}


glm::vec3 RayTracer::ReflectedPointOnTriangle(const Triangle* triangle, glm::vec3 points) const
{
	/// The reflection point on the triangle plane can be calculated as following:

	// 1. construct the plane from 3 points (non-collinear points)
	glm::vec3 p_0 = triangle->GetPoints()[0];
	glm::vec3 p_1 = triangle->GetPoints()[1];
	glm::vec3 p_2 = triangle->GetPoints()[2];

	glm::vec3 n = triangle->GetNormal();

	float b = (n.x * p_1.x) + (n.y * p_1.y) + (n.z * p_1.z);
	// 2. mirror the point from the plane
	float t = (b - (points.x * n.x + points.y * n.y + points.z * n.z)) / (n.x * n.x + n.y * n.y + n.z * n.z); //distance from point to plane
	
	// 
	glm::vec3 m = glm::vec3(points.x + t * n.x, points.y + t * n.y, points.z + t * n.z); // points on mirror

	return  points + 2 * t * n; // reverse average point from average point 
}

bool RayTracer::IsKnifeEdgeDiffraction(const glm::vec3 start_position, const glm::vec3 end_position, std::vector<glm::vec3>& edges_points) const
{
	const unsigned int max_scan = 5; // Maximum Scan
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
			//lastest_hit_position should be in between start_positon and end_position in xz plane
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
	double start_end_to_on_xz_angle = glm::angle(latest_hit_direction, start_end_on_xz_direction);
	float distance_on_xz = latest_hit_distance * cos(start_end_to_on_xz_angle);
	double start_edge_angle = glm::angle(start_end_on_xz_direction, scan_direction);
	float distance_to_edge = distance_on_xz / cos(start_edge_angle);

	// calculate the start_edge_point after exit the obstables
	edge_position = start_position + scan_direction * distance_to_edge;
	return true;
}

glm::vec3 RayTracer::NearestEdgeFromPoint(glm::vec3 point_position, std::vector<glm::vec3>& edges_points) const
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

void RayTracer::CleanEdgePoints(const glm::vec3 start_position, const glm::vec3 end_position, std::vector<glm::vec3>& edges_points) const
{
	if (edges_points.size() == 1) return;

	if (edges_points.size() < 3) {
		for (int i = edges_points.size() - 1; i >= 0; --i)
			for (int j = i - 1; j >= 0; --j) {
				glm::vec3 i_edge = edges_points[i];
				glm::vec3& j_edge = edges_points[j];
				if (glm::distance(i_edge, j_edge) <= .1f) {
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

float RayTracer::CalculateDiffractionByV(float v) const {
	return 6.9f + 20.0 * log10(sqrt(pow(v - 0.1, 2) + 1) + v - 0.1);
}

float RayTracer::CalculateVOfEdge(glm::vec3 start_position, glm::vec3 edge_position, glm::vec3 end_position, float frequency) const {
	
	float wave_length = 3e8 / (frequency);
	glm::vec3 start_to_end_direction = glm::normalize(end_position - start_position);

	glm::vec3 start_to_edge_direction = glm::normalize(edge_position - start_position);
	glm::vec3 end_to_edge_direction = glm::normalize(edge_position - end_position);

	float angle_1 = glm::angle(start_to_edge_direction, start_to_end_direction);
	float angle_2 = glm::angle(end_to_edge_direction, -start_to_end_direction);
	float r1 = glm::distance(start_position, end_position);
	float r2 = glm::distance(end_position, edge_position);
	float s1 = r1 * cos(angle_1);
	float s2 = r2 * cos(angle_2);
	float h = sin(angle_1) * r1;
	float v = h * sqrt((2.0f / wave_length) * (s1 + s2) / (r1 * r2));

	return v;
}

void RayTracer::CalculateCorrectionCosines(	glm::vec3 start_position, std::vector<glm::vec3> edges,
											glm::vec3 end_position, std::pair<float, float>& calculated_cosines) const
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