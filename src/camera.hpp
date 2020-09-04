#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Window;
class GLFWwindow;

enum Direction : int {
	kForward = 0,
	kBackward,
	kLeft,
	kRight,
	kUp,
	kDown
};
enum Rotation : int {
	kYaw = 0,
	kRoll,
	kPitch
};

class Camera {
public:
	// todo:
	// camera speed
	// constructors
	Camera(Window* window);

	void Move(const Direction camera_direction, float delta_time);
	void Rotate(const Rotation camera_rotation, float delta_time);
	void Reset();
	void UpdateProjection();

	void UpdateRotation();

	void UpdateView();
	glm::mat4 GetView() const;

	float yaw_;
	float pitch_;

	float fov_;

	Window* window_;
	float camera_move_speed_;
	float camera_rotate_speed_;

	glm::vec3 position_;
	glm::vec3 front_direction_;
	glm::vec3 up_direction_;

	glm::mat4 projection_;
	glm::mat4 view_;
	float zoom_;
};
#endif // !CAMERA_H