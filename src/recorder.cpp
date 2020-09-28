#include "recorder.hpp"

#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>

Recorder::Recorder(const std::string& record_path) : direct_file(nullptr), ref_file(nullptr), single_edge_file(nullptr), multi_edges_file(nullptr) {
	direct_file = new std::ofstream(record_path + "direct_path.csv");
	if (!direct_file->is_open()) {
		std::cout << "Cannot write direct_file" << std::endl;
		return;
	}
	ref_file = new std::ofstream(record_path + "reflection.csv");
	if (!direct_file->is_open()) {
		std::cout << "Cannot write direct_file" << std::endl;
		direct_file->close();
		return;
	}
	single_edge_file = new std::ofstream(record_path + "single-edge.csv");
	if (!direct_file->is_open()) {
		std::cout << "Cannot write direct_file" << std::endl;
		direct_file->close();
		ref_file->close();
		return;
	}
	multi_edges_file = new std::ofstream(record_path + "multiple-edges.csv");
	if (!direct_file->is_open()) {
		std::cout << "Cannot write direct_file" << std::endl;
		direct_file->close();
		ref_file->close();
		single_edge_file->close();
		return;
	}
	*direct_file		<< "tx_x_pos, tx_y_pos, tx_z_pos, tx_freq, rx_x_pos, rx_y_pos, rx_z_pos, distance, path_loss_dB\n";
	*ref_file			<< "tx_x_pos, tx_y_pos, tx_z_pos, tx_freq, rx_x_pos, rx_y_pos, rx_z_pos, ref_x_pos, ref_y_pos, ref_z_pos, tm_or_te , ref_coeff, d1, d2, angle2norm_deg, ref_loss_dB\n";
	*single_edge_file	<< "tx_x_pos, tx_y_pos, tx_z_pos, tx_freq, rx_x_pos, rx_y_pos, rx_z_pos, edge_x_pos, edge_y_pos, edge_z_pos, d1, d2, v, diff_loss_dB, free_space_loss, total_loss_dB\n";
	*multi_edges_file	<< "tx_x_pos, tx_y_pos, tx_z_pos, tx_freq, rx_x_pos, rx_y_pos, rx_z_pos, near_tx_x_pos, near_tx_y_pos, near_tx_z_pos, center_x_pos, center_y_pos, center_z_pos,"
						<< "near_rx_x_pos, near_rx_y_pos, near_rx_z_pos, d1, d2, d3, d4, v_near_tx, v_cen, v_near_rx, c_v_near_tx, c_v_cen, c_v_near_rx, c1_correct, c2_correct, diff_los_dB, free_space_loss , total_loss_dB\n";
}
Recorder::~Recorder() { 
	direct_file->close();
	ref_file->close();
	single_edge_file->close();
	multi_edges_file->close();
}
void Recorder::RecordDirectPath(const glm::vec3& tx_pos, float tx_freq, const glm::vec3& rx_pos, float distance, float path_loss_dB)
{
	constexpr char cm = ',';
	*direct_file	<< tx_pos.x << cm << tx_pos.y << cm << tx_pos.z << cm << tx_freq << cm << rx_pos.x << cm << rx_pos.y << cm << rx_pos.z << cm
					<< distance << cm << path_loss_dB << std::endl;
}

void Recorder::RecordRefection(const glm::vec3& tx_pos, float tx_freq, const glm::vec3& rx_pos, const glm::vec3& ref_pos, bool TM_or_TE , float coeff, float reflect_loss_dB)
{
	const char t_ = (TM_or_TE)? 'M': 'E';
	constexpr char cm = ',';
	const float angle = glm::degrees(glm::angle(glm::normalize(tx_pos-ref_pos), glm::normalize(rx_pos-ref_pos))) / 2.0f;
	const float d1 = glm::distance(tx_pos, ref_pos);
	const float d2 = glm::distance(rx_pos, ref_pos);
	*ref_file	<< tx_pos.x << cm << tx_pos.y << cm << tx_pos.z << cm << tx_freq << cm << rx_pos.x << cm << rx_pos.y << cm << rx_pos.z << cm
				<< ref_pos.x << cm << ref_pos.y << cm << ref_pos.z << cm << t_ << cm << coeff << cm 
				<< d1 << cm << d2 << cm << angle << cm 
				<< reflect_loss_dB << std::endl;
}

void Recorder::RecordSigleEdge(const glm::vec3& tx_pos, float tx_freq, const glm::vec3& rx_pos, const glm::vec3& edge_pos, float v, float diff_loss_dB, float total_loss_dB)
{
	constexpr char cm = ',';
	float free_space_loss = total_loss_dB - diff_loss_dB;
	*single_edge_file	<< tx_pos.x		<< cm << tx_pos.y	<< cm << tx_pos.z	<< cm << tx_freq << cm << rx_pos.x << cm << rx_pos.y << cm << rx_pos.z << cm
						<< edge_pos.x	<< cm << edge_pos.y << cm << edge_pos.z << cm << glm::distance(tx_pos, edge_pos) << cm << glm::distance(edge_pos, rx_pos) << cm 
						<< v << cm << diff_loss_dB << cm << free_space_loss << cm << total_loss_dB << std::endl;
}

void Recorder::RecordMultipleEdges(	const glm::vec3& tx_pos, float tx_freq, const glm::vec3& rx_pos, 
									const glm::vec3& near_tx_edge, const glm::vec3& center_edge, const glm::vec3& near_rx_edge, 
									float v_near_tx, float v_cen, float v_near_rx,
									float c_v_near_tx , float c_v_cen ,  float c_v_near_rx,
									float c1_correct, float c2_correct , float diff_loss, float total_loss)
{
	constexpr char cm = ',';
	float free_space_loss = total_loss - diff_loss;

	*multi_edges_file	<< tx_pos.x << cm << tx_pos.y << cm << tx_pos.z << cm << tx_freq << cm << rx_pos.x << cm << rx_pos.y << cm << rx_pos.z << cm
						<< near_tx_edge.x << cm << near_tx_edge.y << cm << near_tx_edge.z << cm
						<< center_edge.x << cm << center_edge.y << cm << center_edge.z << cm
						<< near_rx_edge.x << cm << near_rx_edge.y << cm << near_rx_edge.z << cm
						<< glm::distance(tx_pos, near_tx_edge) << cm << glm::distance(near_tx_edge, center_edge) << cm 
						<< glm::distance(center_edge, near_rx_edge) << cm << glm::distance(near_rx_edge, rx_pos) << cm
						<< v_near_tx << cm << v_cen << cm << v_near_rx << cm
						<< c_v_near_tx << cm << c_v_cen << cm << c_v_near_rx << cm << c1_correct << cm << c2_correct << cm
						<< diff_loss << cm << free_space_loss << cm << total_loss << std::endl;
}
