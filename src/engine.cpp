#include "engine.hpp"

#include <iostream>
#include <set>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "window.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "object.hpp"

#include "polygon_mesh.hpp"
#include "cube.hpp"
#include "ray.hpp"

#include "ray_tracer.hpp"
#include "transmitter.hpp"
#include "receiver.hpp"

#include "radiation_pattern.hpp"
#include "transform.hpp"

#include "printer.hpp"
#include "communicator.hpp"

#include "recorder.hpp"

unsigned int Engine::global_engine_id_ = 0;

Engine::Engine():	
							window_(nullptr), 
							engine_id_(++global_engine_id_),
							default_shader_(nullptr),
							main_camera_(nullptr),
							recorder_(nullptr)
{
	main_camera_ = nullptr;
	communicator_ = new Communicator();
}
Engine::Engine(Window* window) :
							window_(window), 
							engine_id_(++global_engine_id_),
							default_shader_(nullptr),
							main_camera_(nullptr),
							recorder_(nullptr)
{
	main_camera_ = new Camera(window_);
	communicator_ = new Communicator();
	AssignWindow(window_);
}

Engine::~Engine()
{
	if (window_ != nullptr) delete window_;
	delete main_camera_;
	delete map_;
}


void Engine::AssignWindow(Window* window)
{
	window_ = window;
	window_->AssignEngine(this);
}

void Engine::Reset()
{
	// Reset the simulation

	// Reset transmitters
	transmitters_.clear();
	// Reset Receivers
	receivers_.clear();
}

void Engine::Run()
{
	for (auto & transmitter : transmitters_) {
	}
}

void Engine::RunWithWindow()
{
	if (window_ == nullptr) {
		std::cout << "Cannot run window without assigning the window\n";
		return;
	}
	window_->Run();
}

unsigned int Engine::GetTransmittersNumber() const
{
	return transmitters_.size();
}

unsigned int Engine::GetReceiversNumber() const
{
	return 0;
}

bool Engine::AddTransmitter(glm::vec3 position, glm::vec3 rotation, float frequency)
{
	if (ray_tracer_ == nullptr) return false;
	auto transmitter = new Transmitter({position, glm::vec3(1.0f) ,rotation }, frequency, ray_tracer_);
	transmitters_.push_back(transmitter);
	return true;
}

void Engine::InitializeWithoutWindow()
{
	LoadMap();
	LoadRayTracer();
}

void Engine::InitalizeWithWindow()
{
	engine_mode_ = EngineMode::kView;
	on_first_right_click_ = true;
	on_first_left_click_ = true;
	on_right_click_ = false;
	on_left_click_ = false;
	LoadObjects();
	LoadShaders();
	LoadTexture();
	LoadMap();
	LoadRayTracer();

	glfwSetWindowUserPointer(window_->GetGLFWWindow(), this);
	glfwSetCursorPosCallback(window_->GetGLFWWindow(), MousePositionCallback);
	glfwSetMouseButtonCallback(window_->GetGLFWWindow(), MouseButtonCallback);
	glfwSetScrollCallback(window_->GetGLFWWindow(), MouseScrollCallback);
}

void Engine::LoadRayTracer()
{
	std::cout << "Loading Ray Tracer" << std::endl;
	ray_tracer_ = new RayTracer(map_);
	pattern_.push_back(RadiationPattern{ "../assets/patterns/pattern-1.txt" });
	//recorder_ = new Recorder("../assets/records/");
}

void Engine::LoadComponents()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	LoadShaders();
	LoadTexture();
	LoadMap();
	LoadRayTracer(); // LoadRayTracer() tracer must be after LoadMap()
	//PrintMap();
}

void Engine::LoadMap()
{
	// Load the map from .obj file
	map_ = new PolygonMesh("../assets/obj/new2.obj", default_shader_, window_ != nullptr);
}

void Engine::LoadObjects()
{

}

void Engine::LoadShaders()
{
	//Load Shader
	Object::default_shader_ = new Shader("../src/shaders/default.vert", "../src/shaders/default.frag");
	Object::ray_shader_ = new Shader("../src/shaders/ray.vert", "../src/shaders/ray.frag");
}

void Engine::LoadTexture()
{
	//Load Texture
	
}


void Engine::Update()
{
	
}

void Engine::PrintMap()
{
	std::cout << "Printing\n";
	Printer printer{ray_tracer_};
	printer.Print("../test.ppm", glm::vec3(0.0f, 8.0f, 0.0f), 2.5e9f, 1.5f );
	//printer.TestPrint("../test-head-map.ppm");
}

void Engine::KeyActions()
{
	GLFWwindow* window = window_->GetGLFWWindow();

	float current_time = glfwGetTime();
	float delta_time = current_time - last_time_;
	last_time_ = current_time;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
		return;
	}

	switch (engine_mode_) {
	case kView:
		KeyViewMode(delta_time);
		break;
	case kMoveObjects:
		KeyMoveMode(delta_time);
	}

}

void Engine::KeyViewMode(float delta_time)
{
	GLFWwindow* window = window_->GetGLFWWindow();
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		main_camera_->Move(Direction::kForward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		main_camera_->Move(Direction::kLeft, delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		main_camera_->Move(Direction::kBackward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		main_camera_->Move(Direction::kRight, delta_time);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		main_camera_->Move(Direction::kUp, delta_time);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		main_camera_->Move(Direction::kDown, delta_time);

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		main_camera_->Reset();

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		main_camera_->Rotate(Rotation::kYaw, delta_time);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		main_camera_->Rotate(Rotation::kYaw, -delta_time);

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		main_camera_->Rotate(Rotation::kPitch, delta_time);
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		main_camera_->Rotate(Rotation::kPitch, -delta_time);

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		engine_mode_ = EngineMode::kMoveObjects;
}

void Engine::KeyMoveMode(float delta_time)
{
	GLFWwindow* window = window_->GetGLFWWindow();
	//std::cin >> 
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		engine_mode_ = EngineMode::kView;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		//test_receiver_->Move(Direction::kForward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		//test_receiver_->Move(Direction::kLeft, delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		//test_receiver_->Move(Direction::kBackward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		//test_receiver_->Move(Direction::kRight, delta_time);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		//test_receiver_->Move(Direction::kUp, delta_time);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		//test_receiver_->Move(Direction::kDown, delta_time);

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		//transmitter_->Move(Direction::kForward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		//transmitter_->Move(Direction::kLeft, delta_time);
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		//transmitter_->Move(Direction::kBackward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		//transmitter_->Move(Direction::kRight, delta_time);

	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		//transmitter_->Rotate(Direction::kLeft, delta_time);
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		//transmitter_->Rotate(Direction::kRight, delta_time);
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		//transmitter_->Rotate(Direction::kUp, delta_time);
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		//transmitter_->Rotate(Direction::kDown, delta_time);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS);
		//transmitter_->ToggleDisplay();
	//if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
	//	ray_tracer_->print_each_ = true;
	//if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
	//	ray_tracer_->print_each_ = false;
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

		main_camera_->Rotate(Rotation::kYaw, xoffset);
		main_camera_->Rotate(Rotation::kPitch, yoffset);
	}
	else {
		on_first_right_click_ = true;
	}
}

void Engine::MouseScroll(double xoffset, double yoffset)
{
	/// Implement later
	//std::cout << "scroll : (" <<xoffset << ", " << yoffset << ")." << std::endl;
	main_camera_->camera_move_speed_ += yoffset;
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
	map_->DrawObject(main_camera_);
	for (const auto& transmitter : transmitters_) {
		transmitter->DrawObjects(main_camera_);
	}

	//test_transmitter_->DrawRadiationPattern(main_camera_);
	//transmitter_->DrawRadiationPattern(main_camera_);
	//transmitter_->DrawObjects(main_camera_);
	//test_receiver_->DrawObjects(main_camera_);
}

void Engine::OnKeys()
{
	KeyActions();
}
