#ifndef RECEIVER_H
#define RECEIVER_H

#include <vector>
#include <glm/glm.hpp>
#include "transform.hpp"

#include "record.hpp" 
#include "camera.hpp"

class RayTracer;
class Transmitter;
class Object;
struct Result;
class Recorder;
class Shader;

class Receiver {

public:

	Receiver(Transform transform, RayTracer * ray_tracer);
	Receiver(Transform transform, RayTracer * ray_tracer, Transmitter * transmitter);
    ~Receiver();

	unsigned int GetID() const;

	void ConnectATransmitter(Transmitter* transmitter);
	void DisconnectATransmitter();
	Transmitter * GetTransmitter() const;
	Transform GetTransform()const;

	float GetReceiverGain(const glm::vec3 & position) const;

	Result GetResult() const;
	glm::vec3 GetPosition() const;

	// Actions
	void MoveTo(glm::vec3 near_rx_position);
	void Move(Direction direction, float delta_time);

	// Visualization
	void UpdateResult();
	void Reset();
	// Visualisation
	std::vector<Object*> rays_;
	Object* object_;

	void Clear();

	void DrawObjects(Camera* main_camera);

	static unsigned int global_id_;

    void InitializeVisualObject(Shader * shader);
	bool IsInitializedObject();
    void VisualUpdate();

private:
	unsigned int id_;
	Result result_;

	std::vector<Record> records_;

	// Variables
	Transform transform_;
	glm::vec3 velocity_;
	float move_speed_;

	// Ray Tracer
	RayTracer * ray_tracer_;
	Transmitter* transmitter_;



    void UpdateVisualRayComponents();
};
#endif //!RECEIVER_H