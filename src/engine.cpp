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
	InitalizeWindowController();
}

Engine::~Engine()
{
	//delete Ray::global_ray_shader_;
	//delete Object::default_shader_;
	delete window_;
	delete main_camera_;
	delete map_;
}


void Engine::AssignWindow(Window* window)
{
	window_ = window;
}

void Engine::Reset()
{
	std::cout << "Engine: Reset the simulation" << std::endl;
}

void Engine::InitalizeWindowController()
{
	engine_mode_ = EngineMode::kView;
	on_first_right_click_ = true;
	on_first_left_click_ = true;
	on_right_click_ = false;
	on_left_click_ = false;
	glfwSetWindowUserPointer(window_->GetGLFWWindow(), this);
	glfwSetCursorPosCallback(window_->GetGLFWWindow(), MousePositionCallback);
	glfwSetMouseButtonCallback(window_->GetGLFWWindow(), MouseButtonCallback);
	glfwSetScrollCallback(window_->GetGLFWWindow(), MouseScrollCallback);
}

void Engine::LoadRayTracer()
{
	std::cout << "Loading Ray Tracer" << std::endl;
	ray_tracer_ = new RayTracer(map_);
	Transform traismitter_trans{glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f)};
	transmitter_ = new Transmitter(traismitter_trans, 2.3e9, ray_tracer_);
	
	// todo: implement multiple radiation patterns
	pattern_ = new RadiationPattern("../assets/patterns/pattern-1.txt");
	// add pattern to transmitter
	std::cout << "added pattern\n";
	transmitter_->AssignRadiationPattern(pattern_);
	Transform receiver_trans{ glm::vec3(9.13651f, 20.0f, 100.0f), glm::vec3(0.0f), glm::vec3(0.0f) };
	//test_receiver_ = new Receiver(receiver_trans, ray_tracer_, transmitter_);
	for (unsigned int i = 0; i < 1; ++i) {
		glm::vec3 random_position = glm::vec3(rand() % 200 - 100.0f, rand() % 1 + 1.5f, rand() % 200 - 100.0f);
		Transform receiver_trans{ random_position, glm::vec3(0.0f), glm::vec3(0.0f) };
		Receiver * receiver = new Receiver(receiver_trans, ray_tracer_, transmitter_);
		transmitter_->AddReceiver(receiver);
	}
	test_receiver_ = transmitter_->GetReceiver(0);
}

void Engine::LoadComponents()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	LoadObjects();
	LoadShaders();
	LoadTexture();
	LoadMap();
	LoadRayTracer(); // LoadRayTracer() tracer must be after LoadMap()
}

void Engine::LoadMap()
{
	std::cout << "Engine:Loading map" << std::endl;
	map_ = new PolygonMesh("../assets/obj/new2.obj", default_shader_);
	Cube * test_cube_1 = new Cube(Transform{ glm::vec3(10.0f, 10.0f, 2.0f), glm::vec3(.5f, 0.5f, .5f), glm::vec3(0.0f) }, default_shader_);
	Cube * test_cube_2 = new Cube(Transform{ glm::vec3(-10.0f, 5.0f, 10.0f), glm::vec3(.5f, 0.5f, .5f), glm::vec3(0.0f) }, default_shader_);


	//Transform transmitter_transform{ glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f) };
	//test_transmitter_ = new Transmitter(transmitter_transform, default_shader_);

	render_objects_.push_back(test_cube_1);
	render_objects_.push_back(test_cube_2);
	render_objects_.push_back(map_);
	//render_objects_.push_back(test_ray_);

}

void Engine::LoadObjects()
{
	std::cout << "Engine:Loading objects" << std::endl;

}

void Engine::LoadShaders()
{
	std::cout << "Engine:Loading shaders" << std::endl;
	default_shader_ = new Shader("../src/shaders/default.vert", "../src/shaders/default.frag");
	Object::default_shader_ = new Shader("../src/shaders/default.vert", "../src/shaders/default.frag");
	Object::ray_shader_ = new Shader("../src/shaders/ray.vert", "../src/shaders/ray.frag");
}

void Engine::LoadTexture()
{
	std::cout << "Engine:Loading textures" << std::endl;
	
}


void Engine::Trace()
{
	glm::vec3 position = { -4.0f, 1.0f, 0.0f };
	glm::vec4 direction = { 1.0f , 0.0f, 0.0f, 1.0f};

	for (float i = 0; i < 360; i += 10) {
		for (float j = 0; j < 360; j += 1) {
			float t = 0;
			auto trans_direction = glm::rotate(glm::mat4(1.0f), glm::radians(i), glm::vec3(0.0f, 1.0f, 0.0f));
			trans_direction = glm::rotate(trans_direction, glm::radians(j), glm::vec3(0.0f, 0.0f, 1.0f));
			auto new_direction = trans_direction * direction;
			glm::vec3 i_direction = glm::vec3{ new_direction.x, new_direction.y, new_direction.z };
			Ray * ray = new Ray(position, i_direction, default_shader_);
			if (map_->IsHit(*ray, t)) {
				//std::cout << i << ".) Hit!! t = " << t << std::endl;
				ray->InitializeRay(t);
				rays_.push_back(ray);
			}
			else {
				//std::cout << i << ".) Doesn't hit" << std::endl;
				delete ray;
			};
			//ray->InitializeRay(10.0f);

		}
	}
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
		test_receiver_->Move(Direction::kForward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		test_receiver_->Move(Direction::kLeft, delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		test_receiver_->Move(Direction::kBackward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		test_receiver_->Move(Direction::kRight, delta_time);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		test_receiver_->Move(Direction::kUp, delta_time);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		test_receiver_->Move(Direction::kDown, delta_time);

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		transmitter_->Move(Direction::kForward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		transmitter_->Move(Direction::kLeft, delta_time);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		transmitter_->Move(Direction::kBackward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		transmitter_->Move(Direction::kRight, delta_time);

	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		transmitter_->Rotate(Direction::kLeft, delta_time);
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		transmitter_->Rotate(Direction::kRight, delta_time);
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		transmitter_->Rotate(Direction::kUp, delta_time);
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		transmitter_->Rotate(Direction::kDown, delta_time);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		transmitter_->ToggleDisplay();
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
	/*for (auto& object : render_objects_) {
		object->DrawObject(main_camera_);
	}*/
	/*for (auto& ray : rays_) {
		ray->DrawObject(main_camera_);
	}*/
	//test_transmitter_->DrawRadiationPattern(main_camera_);
	//transmitter_->DrawRadiationPattern(main_camera_);
	transmitter_->DrawObjects(main_camera_);
	test_receiver_->DrawObjects(main_camera_);
}
