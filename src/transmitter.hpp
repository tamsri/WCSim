#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <string>
#include <vector>
#include <map>
#include "transform.hpp"
#include <glm/matrix.hpp>
#include "camera.hpp"

class Camera;
class Ray;
class Point;
class RadiationPattern;
class RayTracer;
class Object;
class Receiver;
class Triangle;

struct Result;

class Transmitter {
public:
	Transmitter(Transform transform, 
				float frequency, 
				RayTracer * ray_tracer);
	~Transmitter();
	unsigned int GetID() const;
	// Actions
	void ConnectAReceiver(Receiver* receiver);
	void DisconnectAReceiver(unsigned int receiver_id);
	void DrawRadiationPattern(Camera * camera);
	void AssignRadiationPattern(RadiationPattern* pattern);
	void MoveTo(glm::vec3 position);
	void RotateTo(glm::vec3 rotation);
	Transform GetTransform() const;
	float GetFrequency() const;
	float GetAveragePL() const ;
	float GetTransmitterGain(glm::vec3 near_tx_position);
	std::map<unsigned int, Receiver* >& GetReceivers();

	std::string GetReceiversIDs();
	Receiver* GetReceiver(unsigned int index);
	glm::vec3 GetPosition();
	// Movement
	void Move(const Direction direction, float delta_time);
	void Rotate(const Direction rotation, float delta_time);
	void ToggleDisplay();

	void UpdateResult();
	void UpdateRadiationPattern();
	void Reset();
	void Clear();
	// Visualisation
	void DrawObjects(Camera * camera);

	std::vector<Triangle*> hit_triangles;
	static unsigned int global_id_;
private:
	
	// Variables
	unsigned int id_;
	float transmitter_power_output_;
	float frequency_;
	float move_speed_;
	float rotation_speed_;
	float average_path_loss_;

	bool display_pattern_;
	Transform transform_;
	RadiationPattern * current_pattern_;
	RayTracer* ray_tracer_;
	Point* current_point_;
	
	std::map<unsigned int, Receiver* > receivers_;
	std::vector<Result> receiver_results;
	// Visualization
	std::vector<Object* > objects_;

};

#endif