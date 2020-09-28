#ifndef RECORDER_H
#define RECORDER_H
#include <string>
#include <glm/vec3.hpp>

class Recorder {
public:
	Recorder(const std::string& record_path);
	~Recorder();
	void RecordDirectPath(		const glm::vec3& tx_pos , float tx_freq, const glm::vec3 & rx_pos, float distance, float path_loss_dB);
	void RecordRefection(		const glm::vec3 & tx_pos, float tx_freq, const glm::vec3 & rx_pos, const glm::vec3& ref_pos, bool TM_or_TE ,float coeff,  float reflect_loss_dB);
	void RecordSigleEdge(		const glm::vec3& tx_pos, float tx_freq, const glm::vec3& rx_pos, const glm::vec3& edge_pos, float v, float diff_loss_dB, float total_loss_dB);
	void RecordMultipleEdges(	const glm::vec3& tx_pos, float tx_freq, const glm::vec3& rx_pos,
								const glm::vec3& near_tx_edge, const glm::vec3& center_edge, const glm::vec3& near_rx_edge,
								float v_near_tx, float v_cen, float v_near_rx,
								float c_v_near_tx, float c_v_cen, float c_v_near_rx,
								float c1_correct, float c2_correct, float diff_loss_dB, float total_loss_dB);
private:
	std::ofstream* direct_file;
	std::ofstream* ref_file;
	std::ofstream* single_edge_file;
	std::ofstream* multi_edges_file;
};
#endif // !RECORDER_H
