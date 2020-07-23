#include "engine.hpp"

#include <iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include "window.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "polygon_mesh.hpp"


unsigned int Engine::global_engine_id_ = 0;

Engine::Engine():	
							window_(nullptr), 
							engine_id_(++global_engine_id_),
							default_shader_(nullptr),
							main_camera_(nullptr)
{
	main_camera_ = nullptr;
	
	std::cout << "Initalize Engine without window. (ID: " << engine_id_ << ")." << std::endl;
}
Engine::Engine(Window* window) :
							window_(window), 
							engine_id_(++global_engine_id_),
							default_shader_(nullptr),
							main_camera_(nullptr)
{
	main_camera_ = new Camera(window_);
	std::cout << "Initialize Engine with window. (ID: " << engine_id_ << ")." << std::endl;
}

void Engine::AssignWindow(Window* window)
{
	window_ = window;
}

void Engine::InitalizeWindowController()
{
	engine_mode_ = kView;
	on_first_right_click_ = true;
	on_first_left_click_ = true;
	on_right_click_ = false;
	on_left_click_ = false;
	glfwSetWindowUserPointer(window_->GetGLFWWindow(), this);
	glfwSetCursorPosCallback(window_->GetGLFWWindow(), MousePositionCallback);
	glfwSetMouseButtonCallback(window_->GetGLFWWindow(), MouseButtonCallback);
}

void Engine::LoadComponents()
{
	LoadMap();
	LoadObjects();
	LoadShaders();
	LoadTexture();
}

void Engine::LoadMap()
{
	std::cout << "Engine:Loading map" << std::endl;
	PolygonMesh * map = new PolygonMesh("../assets/obj/map.obj");
}

void Engine::LoadObjects()
{
	std::cout << "Engine:Loading objects" << std::endl;

}

void Engine::LoadShaders()
{
	std::cout << "Engine:Loading shaders" << std::endl;
	default_shader_ = new Shader("../src/shaders/default.vert", "../src/shaders/default.frag");
}

void Engine::LoadTexture()
{
	std::cout << "Engine:Loading textures" << std::endl;

}

void Engine::InitializeRays()
{
	std::cout << "Engine:Initializing rays" << std::endl;

}

void Engine::InitializeVoxels()
{
	std::cout << "Engine:Initializing voxels" << std::endl;

}

void Engine::TracePathFrom(glm::vec3 position)
{
	std::cout << "Engine: Tracing Path" << std::endl;

}

void Engine::Update()
{
	KeyActions(); // Get key from each loop
}

void Engine::KeyActions()
{
	GLFWwindow* window = window_->GetGLFWWindow();

	float current_time = glfwGetTime();
	float delta_time = current_time - last_time_;
	last_time_ = current_time;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		std::cout << "wow\n";

		glfwSetWindowShouldClose(window, true);
		return;
	}

	switch (engine_mode_) {
	case kView:
		KeyViewMode(delta_time);
		break;
	}

}

void Engine::KeyViewMode(float delta_time)
{
	GLFWwindow* window = window_->GetGLFWWindow();
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		main_camera_->Move(CameraDirection::kForward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		main_camera_->Move(CameraDirection::kLeft, delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		main_camera_->Move(CameraDirection::kBackward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		main_camera_->Move(CameraDirection::kRight, delta_time);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		main_camera_->Move(CameraDirection::kUp, delta_time);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		main_camera_->Move(CameraDirection::kDown, delta_time);

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		main_camera_->Reset();

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		main_camera_->Rotate(CameraRotation::kYaw, delta_time);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		main_camera_->Rotate(CameraRotation::kYaw, -delta_time);

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		main_camera_->Rotate(CameraRotation::kPitch, delta_time);
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		main_camera_->Rotate(CameraRotation::kPitch, -delta_time);
}

void Engine::MousePosition(double xpos, double ypos)
{
	if (on_right_click_) {

		if (on_first_right_click_) {
			last_mouse_x_pos_ = xpos;
			last_mouse_y_pos_ = ypos;
			on_first_right_click_ = false;
		}

		float xoffset = (xpos - last_mouse_x_pos_) / 50000.0f;
		float yoffset = (last_mouse_y_pos_ - ypos) / 50000.0f;

		main_camera_->Rotate(CameraRotation::kYaw, xoffset);
		main_camera_->Rotate(CameraRotation::kPitch, yoffset);
	}
	else {
		on_first_right_click_ = true;
	}
}

void Engine::MouseScroll(double xoffset, double yoffset)
{
	/// Implement later
}

void Engine::MouseBottonToggler(MouseBottons action)
{
	switch (action) {
	case kRightPress:
		on_right_click_ = true;
		break;
	case kRightRelease:
		on_right_click_ = false;
		break;
	case kLeftPress:
		on_left_click_ = true;
		break;
	case kLeftRelease:
		on_left_click_ = false;
		break;
	}
}

void Engine::MousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	Engine* my_engine = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
	my_engine->MousePosition(xpos, ypos);
}

void Engine::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Engine* my_engine = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
	my_engine->MouseScroll(xoffset, yoffset);
}

void Engine::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	Engine* my_engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS) {
		my_engine->MouseBottonToggler(kRightPress);
	}
	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE) {
		my_engine->MouseBottonToggler(kRightRelease);
	}
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
		my_engine->MouseBottonToggler(kLeftPress);
	}
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
		my_engine->MouseBottonToggler(kLeftRelease);
	}
}


void Engine::Visualize() 
{
}

void Engine::Destroy()
{
	delete window_;
}
