#include "engine.hpp"

#include <map>
#include <iostream>
#include <utility>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "window.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "object.hpp"

#include "polygon_mesh.hpp"
#include "ray.hpp"

#include "ray_tracer.hpp"
#include "transmitter.hpp"
#include "receiver.hpp"

#include "radiation_pattern.hpp"

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
	delete window_;
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
	// Reset transmitters
	for (auto & transmitter : transmitters_){
	    delete transmitter.second;
	}
	transmitters_.clear();
	// Reset Receivers
	for (auto & receiver : receivers_) {
		delete receiver.second;
	}
	receivers_.clear();
}

void Engine::Run()
{
}

void Engine::RunWithWindow()
{
	if (window_ == nullptr) {
		std::cout << "Cannot run window without assigning the window\n";
		return;
	}
	window_->Run();
}

std::string Engine::GetTransmittersList() const
{
	if(transmitters_.empty()) return "0";
	std::string answer = std::to_string(transmitters_.size()) + '&';
	/*for (auto & [id, transmitter] : transmitters_) {
		if (transmitter != nullptr)
			answer += std::to_string(id) + ',';
	}*/
    for(auto itr = transmitters_.begin();itr != transmitters_.end(); ++itr){
        answer += std::to_string(itr->first) + ',';
    }
	answer.pop_back();
	return answer;
}


std::string Engine::GetReceiversList() const
{
	if (receivers_.empty()) return "0";
	std::string answer = std::to_string(receivers_.size()) + '&';
	/*for (auto & [id, receiver] : receivers_) {
			answer += std::to_string(id) + ',';
	}*/
	for(auto itr = receivers_.begin();itr != receivers_.end(); ++itr){
	    answer += std::to_string(itr->first) + ',';
	}
	answer.pop_back();
	return answer;
}

std::string Engine::GetTransmitterInfo(unsigned int transmitter_id)
{
	// Get the transmitter 
	Transmitter * tx;
	if (transmitters_.find(transmitter_id) != transmitters_.end())
        tx = transmitters_.find(transmitter_id)->second;
	else
	    return "-1";

	std::string answer = std::to_string(transmitter_id) + ":";
	// ID : Position : Rotation : Frequency : Receiver N & Receivers' IDs : Average Path Loss
	const Transform& tx_trans = tx->GetTransform();
	const glm::vec3& tx_pos = tx_trans.position;
	const glm::vec3& tx_rot = tx_trans.rotation;
	answer +=	std::to_string(tx_pos.x) + "," +
				std::to_string(tx_pos.y) + "," + 
				std::to_string(tx_pos.z) + ":" + 
				std::to_string(tx_rot.x) + "," +
				std::to_string(tx_rot.y) + "," +
				std::to_string(tx_rot.z) + ":" +
				std::to_string(tx->GetFrequency()) + ":" +
				tx->GetReceiversIDs();
	return std::string(answer);
}


std::string Engine::GetReceiverInfo(unsigned int receiver_id)
{
	// Get the transmitter 
	Receiver* rx;
    if (receivers_.find(receiver_id) != receivers_.end()){
        rx = receivers_.find(receiver_id)->second;
    }else return "-1";

	std::string answer = std::to_string(receiver_id) + ":";

	// ID : Position : Rotation : Frequency : Receiver N & Receivers' IDs : Average Path Loss
	auto rx_trans = rx->GetTransform();
	auto rx_pos = rx_trans.position;
	auto rx_rot = rx_trans.rotation;
	answer +=	 std::to_string(rx_pos.x) + "," +
				std::to_string(rx_pos.y) + "," +
				std::to_string(rx_pos.z);

	// Check the transmitter
	auto connected_tx = rx->GetTransmitter();
	if (connected_tx == nullptr) return std::string(answer);

    answer += ":" + std::to_string(connected_tx->GetID());
    rx->UpdateResult();
    // Giving out result
    std::cout << "giving result.\n";
    if (rx->GetResult().is_valid)
        answer += ":" + std::to_string(rx->GetResult().total_attenuation);

	return std::string(answer);
}

void Engine::ExecuteCommand(ip::tcp::socket& socket, boost::system::error_code & ign_err, std::string & command)
{
	switch (command[1]) {
	case '1': {
		// Add a station to the environment
		std::cout << "Server: The client wants to add a transmitter.\n";
		boost::asio::write(socket, boost::asio::buffer("cok"), ign_err);

		std::string input_data = command.substr(3);
		std::vector<std::string> splitted_inputs;
		boost::split(splitted_inputs, input_data, boost::is_any_of(":"));

		std::vector<std::string> splitted_data;
		// Get location from input string
		std::string location_string = splitted_inputs[0];
		boost::split(splitted_data, location_string, boost::is_any_of(","));
		glm::vec3 position = glm::vec3(std::stof(splitted_data[0]),
			std::stof(splitted_data[1]),
			std::stof(splitted_data[2]));
		splitted_data.clear();
		// Get rotation from input string
		std::string rotation_string = splitted_inputs[1];
		boost::split(splitted_data, rotation_string, boost::is_any_of(","));
		glm::vec3 rotation = glm::vec3(std::stof(splitted_data[0]),
			std::stof(splitted_data[1]),
			std::stof(splitted_data[2]));
		// Get frequency
		float frequency = std::stof(splitted_inputs[2]);

		if (this->AddTransmitter(position, rotation, frequency))
			boost::asio::write(socket, boost::asio::buffer("suc"), ign_err);
		else
			boost::asio::write(socket, boost::asio::buffer("fai"), ign_err);
	}break;
	case '2': {
		// Add a user to the environment
		std::cout << "Server: The client wants to add a receiver.\n";
		boost::asio::write(socket, boost::asio::buffer("cok"), ign_err);

		// Get location from input string
		std::string location_string = command.substr(3);

		std::vector<std::string> splitted_data;
		boost::split(splitted_data, location_string, boost::is_any_of(","));

		glm::vec3 position = glm::vec3(std::stof(splitted_data[0]),
			std::stof(splitted_data[1]),
			std::stof(splitted_data[2]));
		if (this->AddReceiver(position))
			boost::asio::write(socket, boost::asio::buffer("suc"), ign_err);
		else
			boost::asio::write(socket, boost::asio::buffer("fai"), ign_err);
	}break;
	case '3': {
		// Add a user to a station 
		std::cout << "Server: The client wants to connect a receiver to a station.\n";
		boost::asio::write(socket, boost::asio::buffer("cok"), ign_err);
		std::string input_data = command.substr(3);

		std::vector<std::string> splitted_data;
		boost::split(splitted_data, input_data, boost::is_any_of(","));

		if (this->ConnectReceiverToTransmitter(std::stoul(splitted_data[0]),
			std::stoul(splitted_data[1])))
			boost::asio::write(socket, boost::asio::buffer("suc"), ign_err);
		else
			boost::asio::write(socket, boost::asio::buffer("fai"), ign_err);
		
	}break;
	case '4': {
		// Move a station
		std::cout << "Server: The client wants to move a station.\n";
		boost::asio::write(socket, boost::asio::buffer("cok"), ign_err);

		std::string input_data = command.substr(3);
		std::vector<std::string> splitted_inputs;
		boost::split(splitted_inputs, input_data, boost::is_any_of(":"));

		unsigned int transmitter_id = std::stoul(splitted_inputs[0]);

		std::vector<std::string> splitted_data;
		// Get location from input string
		std::string location_string = splitted_inputs[1];
		boost::split(splitted_data, location_string, boost::is_any_of(","));
		glm::vec3 position = glm::vec3(std::stof(splitted_data[0]),
			std::stof(splitted_data[1]),
			std::stof(splitted_data[2]));
		splitted_data.clear();
		// Get rotation from input string
		std::string rotation_string = splitted_inputs[2];
		boost::split(splitted_data, rotation_string, boost::is_any_of(","));
		glm::vec3 rotation = glm::vec3(std::stof(splitted_data[0]),
			std::stof(splitted_data[1]),
			std::stof(splitted_data[2]));
		// Command the engine
		if (this->MoveTransmitterTo(transmitter_id, position, rotation))
			boost::asio::write(socket, boost::asio::buffer("suc"), ign_err);
		else
			boost::asio::write(socket, boost::asio::buffer("fai"), ign_err);
	}break;
	case '5': {
		// Remove a station
		std::cout << "Server: The client wants to remove a station.\n";
		boost::asio::write(socket, boost::asio::buffer("cok"), ign_err);
		// Get id from input
		std::string input_data = command.substr(3);
		unsigned int station_id = std::stoul(input_data);
		// Command the engine
		if (this->RemoveTransmitter(station_id))
			boost::asio::write(socket, boost::asio::buffer("suc"), ign_err);
		else
			boost::asio::write(socket, boost::asio::buffer("fai"), ign_err);
	}break;
	case '6': {
		// Remove a user
		std::cout << "Server: The client wants to remove a user.\n";
		boost::asio::write(socket, boost::asio::buffer("cok"), ign_err);
		// Get id from input
		std::string input_data = command.substr(3);
		unsigned int user_id = std::stoul(input_data);
		// Command the engine
		if (this->RemoveReceiver(user_id))
			boost::asio::write(socket, boost::asio::buffer("suc"), ign_err);
		else
			boost::asio::write(socket, boost::asio::buffer("fai"), ign_err);
	}break;
	case '7': {
		// Disconnect a user from station
		std::cout << "Server: The client wants to disconenct a user from a station.\n";
		boost::asio::write(socket, boost::asio::buffer("cok"), ign_err);
		// Get ids from input
		std::string input_data = command.substr(3);
		std::vector<std::string> splitted_inputs;
		boost::split(splitted_inputs, input_data, boost::is_any_of(":"));
		unsigned int station_id = std::stoul(splitted_inputs[0]);
		unsigned int user_id = std::stoul(splitted_inputs[1]);
		if (this->DisconnectReceiverFromTransmitter(station_id, user_id))
			boost::asio::write(socket, boost::asio::buffer("suc"), ign_err);
		else
			boost::asio::write(socket, boost::asio::buffer("fai"), ign_err);
	}break;
	case '8': {
		// Move a user to a location
		std::cout << "Server: The client wants to move a user\n";
		boost::asio::write(socket, boost::asio::buffer("cok"), ign_err);
		// Get id and locations
		std::string input_data = command.substr(3);
		std::vector<std::string> splitted_inputs;
		boost::split(splitted_inputs, input_data, boost::is_any_of(":"));
		// Get ID
		unsigned int user_id = std::stoul(splitted_inputs[0]);
		// Get Location
		std::string position_string = splitted_inputs[1];
		std::vector<std::string> splitted_position;
		boost::split(splitted_position, position_string, boost::is_any_of(","));
		glm::vec3 position = glm::vec3(std::stof(splitted_position[0]),
			std::stof(splitted_position[1]),
			std::stof(splitted_position[2]));
		if (this->MoveReceiverTo(user_id, position))
			boost::asio::write(socket, boost::asio::buffer("suc"), ign_err);
		else
			boost::asio::write(socket, boost::asio::buffer("fai"), ign_err);
	}break;
	default: {
        std::cout << "Server: Unknown Command\n";
        boost::asio::write(socket, boost::asio::buffer("cnok"), ign_err);
        boost::asio::write(socket, boost::asio::buffer("fai"), ign_err);
    } break;
	}
}

void Engine::ExecuteQuestion(ip::tcp::socket& socket, boost::system::error_code& ign_err, std::string& question)
{
	switch (question[1]) {
	case '1': {
		// How many stations are in the environment, who are they?
		std::cout << "Server: The Client asks How many transmitter?.\n";
		boost::asio::write(socket, boost::asio::buffer("qok"), ign_err);
		std::string answer = "a:" + this->GetTransmittersList();
		boost::asio::write(socket, boost::asio::buffer(answer), ign_err);
	} break;
	case '2': {
		// How many users are in the environment, who are they?
		std::cout << "Server: The client asks How many receivers?.\n";
		boost::asio::write(socket, boost::asio::buffer("qok"), ign_err);
		std::string answer = "a:" + this->GetReceiversList();
		boost::asio::write(socket, boost::asio::buffer(answer), ign_err);
	}break;
	case '3': {
		// Give me info of the station id #..
		std::cout << "Server: The client asks about a transmitter.\n";
		boost::asio::write(socket, boost::asio::buffer("qok"), ign_err);
		unsigned int id = std::stoi(question.substr(2));
		std::string answer = "a:" + this->GetTransmitterInfo(id);
		boost::asio::write(socket, boost::asio::buffer(answer), ign_err);
	} break;
	case '4': {
		// Give me information of the user number #..
		std::cout << "Server: The client asks about a user.\n";
		boost::asio::write(socket, boost::asio::buffer("qok"), ign_err);
		unsigned int id = std::stoi(question.substr(2));
		std::string answer = "a:" + this->GetReceiverInfo(id);
		boost::asio::write(socket, boost::asio::buffer(answer), ign_err);
	}break;
	default: {
		std::cout << "Server: Unknown Question\n";
		boost::asio::write(socket, boost::asio::buffer("qnok"), ign_err);
		boost::asio::write(socket, boost::asio::buffer("fai"), ign_err);
	}break;
	}
}

bool Engine::AddTransmitter(glm::vec3 position, glm::vec3 rotation, float frequency)
{
	if (ray_tracer_ == nullptr) return false;
	auto * transmitter = new Transmitter({position, glm::vec3(1.0f) ,rotation },
                                                frequency, 0 ,ray_tracer_);
	transmitters_[transmitter->GetID()] = transmitter;
	transmitters_.insert(std::make_pair(transmitter->GetID(), transmitter));
	return true;
}

bool Engine::AddReceiver(glm::vec3 position)
{
	if (ray_tracer_ == nullptr) return false;
	auto * receiver = new Receiver({ position, glm::vec3(1.0f) , glm::vec3(0.0f) }, ray_tracer_);
	receivers_.insert(std::make_pair(receiver->GetID(), receiver));
	return true;
}

bool Engine::RemoveTransmitter(unsigned int transmitter_id)
{
    if(transmitters_.find(transmitter_id) == transmitters_.end()) return false;
	Transmitter* tx =  transmitters_.find(transmitter_id)->second;
	// Disconnect all receivers in the transmitter
	for (auto& [id, rx] : tx->GetReceivers()) {
	    if(rx == nullptr) continue;
		rx->DisconnectATransmitter();
	}
	transmitters_.erase(transmitter_id);
	// delete the transmitter
	delete tx;
	return true;
}

bool Engine::RemoveReceiver(unsigned int receiver_id)
{
    if(receivers_.find(receiver_id) == receivers_.end())return false;
	Receiver* rx = receivers_.find(receiver_id)->second;
	Transmitter* tx = rx->GetTransmitter();
	// Disconnect from the transmitter
	if (tx != nullptr) {
        DisconnectReceiverFromTransmitter(tx->GetID(), rx->GetID());
	}
	// Remove from the engine list
	receivers_.erase(receiver_id);
	delete rx;
	return true;
}

bool Engine::ConnectReceiverToTransmitter(unsigned int tx_id, unsigned int rx_id)
{
    if( transmitters_.find(tx_id) == transmitters_.end() ||
        receivers_.find(rx_id) == receivers_.end())  return false;
    Transmitter * tx = transmitters_.find(tx_id)->second;
    Receiver* rx = receivers_.find(rx_id)->second;
	tx->ConnectAReceiver(rx);
	return true;
}

bool Engine::DisconnectReceiverFromTransmitter(unsigned int tx_id, unsigned int rx_id)
{
    if( transmitters_.find(tx_id) == transmitters_.end() ||
        receivers_.find(rx_id) == receivers_.end())  return false;
	Transmitter * tx = transmitters_.find(tx_id)->second;
	Receiver* rx = receivers_.find(rx_id)->second;
	tx->DisconnectAReceiver(rx_id);
	rx->DisconnectATransmitter();
	return true;
}

bool Engine::MoveTransmitterTo(unsigned int id, glm::vec3 position, glm::vec3 rotation)
{
	if(transmitters_.find(id) == transmitters_.end()) return false;
    Transmitter* tx = transmitters_.find(id)->second;
	tx->MoveTo(position);
	tx->RotateTo(rotation);
	return true;
}

bool Engine::MoveReceiverTo(unsigned int rx_id, glm::vec3 position)
{
    if(receivers_.find(rx_id) == receivers_.end()) return false;
	Receiver* rx = receivers_.find(rx_id)->second;
	rx->MoveTo(position);
	return true;
}

void Engine::InitializeWithoutWindow()
{
	LoadMap();
	LoadRayTracer();
}

void Engine::InitializeWithWindow()
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
	pattern_.emplace_back( "../assets/patterns/pattern-1.txt" );
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

void Engine::MousePosition(double x_pos, double y_pos)
{
	if (on_right_click_) {

		if (on_first_right_click_) {
			last_mouse_x_pos_ = x_pos;
			last_mouse_y_pos_ = y_pos;
			on_first_right_click_ = false;
		}

		float x_offset = (x_pos - last_mouse_x_pos_) / 50000.0f;
		float y_offset = (last_mouse_y_pos_ - y_pos) / 50000.0f;

		main_camera_->Rotate(Rotation::kYaw, x_offset);
		main_camera_->Rotate(Rotation::kPitch, y_offset);
	}
	else {
		on_first_right_click_ = true;
	}
}

void Engine::MouseScroll(double xoffset, double yoffset)
{
	///  Todo: Implement later
	main_camera_->camera_move_speed_ += yoffset;
}

void Engine::MouseButtonToggle(MouseBottons action)
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
        my_engine->MouseButtonToggle(kRightPress);
	}
	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE) {
        my_engine->MouseButtonToggle(kRightRelease);
	}
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
        my_engine->MouseButtonToggle(kLeftPress);
	}
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
        my_engine->MouseButtonToggle(kLeftRelease);
	}
}


void Engine::Visualize() 
{
	map_->DrawObject(main_camera_);
	for (const auto & [id, transmitter] : transmitters_) {
		transmitter->DrawObjects(main_camera_);
	}
}

void Engine::OnKeys()
{
	KeyActions();
}
