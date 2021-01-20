#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "transform.hpp"
#include <glm/matrix.hpp>
#include "camera.hpp"

class Camera;
class Ray;
class RadiationPattern;
class RayTracer;
class Object;
class Receiver;
class Triangle;
class Shader;

struct Result;

class Transmitter {
public:
	Transmitter(Transform transform, 
				float frequency,
				float transmit_power,
				RayTracer * ray_tracer);
	~Transmitter();
	unsigned int GetID() const;
	// Actions
	void ConnectAReceiver(Receiver* receiver);
	void DisconnectAReceiver(unsigned int receiver_id);
	void AssignRadiationPattern(RadiationPattern* pattern);
	void MoveTo(glm::vec3 position);
	void RotateTo(glm::vec3 rotation);
	Transform GetTransform() const;
	float GetFrequency() const;
	float GetTransmitPower() const;
	float GetTransmitterGain(glm::vec3 near_tx_position);
	std::unordered_map<unsigned int, Receiver* >& GetReceivers();

	std::string GetReceiversIDs();
	glm::vec3 GetPosition();
	// Movement
	void Move(Direction direction, float delta_time);
	void Rotate(Direction rotation, float delta_time);

	void UpdateResult();
	void UpdateResultWithVisual();
	void Reset();
	void Clear();

	bool IsInitializedObject();

	// Visualisation
	void InitializeVisualObject(Shader * shader);
	void DrawObject(Camera * camera);
    void VisualUpdate();
	// Visualization
	Object* object_;


    std::unordered_map<unsigned int, Receiver* > receivers_;

    // Global Variables
    static unsigned int global_id_;
private:
	
	// Variables
	unsigned int id_;
	float transmit_power_;
	float frequency_;

	float move_speed_; // for controller
	float rotation_speed_; // for controller

	Transform transform_;
	RadiationPattern * current_pattern_;
	RayTracer* ray_tracer_;

};

#endif