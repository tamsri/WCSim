#include "engine.hpp"

#include <thread>
#include <map>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <utility>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtx/string_cast.hpp>

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
#include "console_controller.hpp"

#include "record.hpp"

unsigned int Engine::global_engine_id_ = 0;

Engine::Engine():	
							window_(nullptr), 
							engine_id_(++global_engine_id_),
							default_shader_(nullptr),
							main_camera_(nullptr),
							recorder_(nullptr),
							ray_tracer_(nullptr),
							on_pressed_(false)
{
	communicator_ = new Communicator();
	console_controller_ = new ConsoleController(this);
}
Engine::Engine(Window* window) :
							window_(window), 
							engine_id_(++global_engine_id_),
							default_shader_(nullptr),
							main_camera_(new Camera(window)),
							recorder_(nullptr),
							ray_tracer_(nullptr),
							on_pressed_(false)
{
	communicator_ = new Communicator();
	console_controller_ = new ConsoleController(this);
	// Assign engine to window.
	window_->AssignEngine(this);
}

Engine::~Engine()
{
	delete window_;
	delete main_camera_;
	delete map_;
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
	std::thread (&ConsoleController::Run, console_controller_).detach();
	window_->Run();
}

std::string Engine::GetTransmittersList() const
{
	if(transmitters_.empty()) return "0";
	std::string answer = std::to_string(transmitters_.size()) + '&';

    for(const auto & transmitter : transmitters_){
        answer += std::to_string(transmitter.first) + ',';
    }
	answer.pop_back();
	return answer;
}


std::string Engine::GetReceiversList() const
{
	if (receivers_.empty()) return "0";
	std::string answer = std::to_string(receivers_.size()) + '&';
	for(const auto & receiver : receivers_){
	    answer += std::to_string(receiver.first) + ',';
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
    std::stringstream answer;
    answer.precision(8);
	answer << transmitter_id << ":";
	// ID : Position : Rotation : Frequency : Receiver N & Receivers' IDs : Average Path Loss
	const Transform& tx_trans = tx->GetTransform();
	const glm::vec3& tx_pos = tx_trans.position;
	const glm::vec3& tx_rot = tx_trans.rotation;
	answer <<	std::scientific << tx_pos.x << "," <<
				std::scientific << tx_pos.y << "," <<
				std::scientific << tx_pos.z << ":" <<
				std::scientific << tx_rot.x << "," <<
				std::scientific << tx_rot.y << ":" <<
				std::scientific << tx->GetFrequency() << ":" <<
				std::scientific << tx->GetTransmitPower() << ":" <<
				tx->GetReceiversIDs();
	return answer.str();
}


std::string Engine::GetReceiverInfo(unsigned int receiver_id)
{
	// Get the transmitter 
	Receiver* rx;
    if (receivers_.find(receiver_id) != receivers_.end()){
        rx = receivers_.find(receiver_id)->second;
    }else return "-1";
    // #0 - ID
    std::stringstream answer;
    answer.precision(8);

	answer  << receiver_id << ":";

	// ID : Position : Rotation : Station ID : ...
	// (total rec pow: tx pow: attenuation : direct or diff :  )
	auto rx_trans = rx->GetTransform();
	auto rx_pos = rx_trans.position;
	auto rx_rot = rx_trans.rotation;
    // #1 - Position
	answer <<	std::scientific << rx_pos.x << ","  <<
				std::scientific << rx_pos.y  << "," <<
				std::scientific << rx_pos.z << ":";
    // #2 - Rotation
    answer <<   std::scientific << rx_rot.x << "," <<
                std::scientific << rx_rot.y;
	// Check the transmitter
	auto connected_tx = rx->GetTransmitter();
	if (connected_tx == nullptr) return answer.str();

	// #3 - Station ID
    answer << ":" << connected_tx->GetID();

    auto result = rx->GetResult();
    // Giving out result
    if (result.is_valid) {
        answer << ":" << std::scientific << result.total_received_power;
        answer << ":" << std::scientific << result.transmit_power;
        answer << ":" << std::scientific << result.total_attenuation;
        if (result.is_los) {
            // Transmitting the direct result.
            answer << ":t";
            answer << "," << std::scientific << result.direct.direct_loss;
            answer << "," << std::scientific << result.direct.tx_gain;
            answer << "," << std::scientific << result.direct.rx_gain;
            answer << "," << std::scientific << result.direct.delay;
        }else{
            // Transmitting the diffraction result.
            answer << ":f";
            answer << "," << std::scientific << result.diffraction.diffraction_loss;
            answer << "," << std::scientific << result.diffraction.tx_gain;
            answer << "," << std::scientific << result.diffraction.rx_gain;
            answer << "," << std::scientific << result.diffraction.delay;
        }
        answer << ":" << result.reflections.size();
        // Transmitting the reflection results.
        for(const auto & reflection: result.reflections ){
            answer << "&" << std::scientific << reflection.reflection_loss;
            answer << "," << std::scientific << reflection.tx_gain;
            answer << "," << std::scientific << reflection.rx_gain;
            answer << "," << std::scientific << reflection.delay;
        }
    }
	return answer.str();
}

void Engine::ExecuteCommand(ip::tcp::socket& socket, boost::system::error_code & ign_err, std::string & command)
{
    auto input_data = command.substr(3);
	switch (command[1]) {
	case '1': {
		// Add a station to the environment
		std::cout << "Server: The client wants to add a transmitter.\n";

		std::vector<std::string> split_inputs;
		boost::split(split_inputs, input_data, boost::is_any_of(":"));

		std::vector<std::string> split_data;
		// Get location from input string
		std::string location_string = split_inputs[0];
		boost::split(split_data, location_string, boost::is_any_of(","));
		glm::vec3 position = glm::vec3(std::stof(split_data[0]),
                                        std::stof(split_data[1]),
                                        std::stof(split_data[2]));
		split_data.clear();
		// Get rotation from input string
		std::string rotation_string = split_inputs[1];
		boost::split(split_data, rotation_string, boost::is_any_of(","));
		glm::vec3 rotation = glm::vec3(std::stof(split_data[0]),
			std::stof(split_data[1]),
			std::stof(split_data[2]));
		// Get frequency
		float frequency = std::stof(split_inputs[2]);

		if (this->AddTransmitter(position, rotation, frequency))
			boost::asio::write(socket, boost::asio::buffer("suc"), ign_err);
		else
			boost::asio::write(socket, boost::asio::buffer("fai"), ign_err);
	}break;
	case '2': {
		// Add a user to the environment
		std::cout << "Server: The client wants to add a receiver.\n";

		std::vector<std::string> split_data;
		boost::split(split_data, input_data, boost::is_any_of(","));

		glm::vec3 position = glm::vec3(std::stof(split_data[0]),
			std::stof(split_data[1]),
			std::stof(split_data[2]));
		if (this->AddReceiver(position))
			boost::asio::write(socket, boost::asio::buffer("suc"), ign_err);
		else
			boost::asio::write(socket, boost::asio::buffer("fai"), ign_err);
	}break;
	case '3': {
		// Connect a user to a station
		std::cout << "Server: The client wants to connect a receiver to a station.\n";
		std::vector<std::string> split_data;
		boost::split(split_data, input_data, boost::is_any_of(","));

		if (this->ConnectReceiverToTransmitter(std::stoul(split_data[0]),
			std::stoul(split_data[1])))
			boost::asio::write(socket, boost::asio::buffer("suc"), ign_err);
		else
			boost::asio::write(socket, boost::asio::buffer("fai"), ign_err);
		
	}break;
	case '4': {
		// Move a station
		std::cout << "Server: The client wants to move a station.\n";
		std::vector<std::string> split_inputs;
		boost::split(split_inputs, input_data, boost::is_any_of(":"));

		unsigned int transmitter_id = std::stoul(split_inputs[0]);

		std::vector<std::string> splitted_data;
		// Get location from input string
		std::string location_string = split_inputs[1];
		boost::split(splitted_data, location_string, boost::is_any_of(","));
		glm::vec3 position = glm::vec3(std::stof(splitted_data[0]),
			std::stof(splitted_data[1]),
			std::stof(splitted_data[2]));
		splitted_data.clear();
		// Get rotation from input string
		std::string rotation_string = split_inputs[2];
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
		// Get id from input
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
		// Get id from input
		unsigned int user_id = std::stoul(input_data);
		// Command the engine
		if (this->RemoveReceiver(user_id))
			boost::asio::write(socket, boost::asio::buffer("suc"), ign_err);
		else
			boost::asio::write(socket, boost::asio::buffer("fai"), ign_err);
	}break;
	case '7': {
		// Disconnect a user from station
		std::cout << "Server: The client wants to disconnect a user from a station.\n";
		// Get ids from input
		std::vector<std::string> split_inputs;
		boost::split(split_inputs, input_data, boost::is_any_of(":"));
		unsigned int station_id = std::stoul(split_inputs[0]);
		unsigned int user_id = std::stoul(split_inputs[1]);
		if (this->DisconnectReceiverFromTransmitter(station_id, user_id))
			boost::asio::write(socket, boost::asio::buffer("suc"), ign_err);
		else
			boost::asio::write(socket, boost::asio::buffer("fai"), ign_err);
	}break;
	case '8': {
		// Move a user to a location
		std::cout << "Server: The client wants to move a user.\n";
		std::vector<std::string> split_inputs;
		boost::split(split_inputs, input_data, boost::is_any_of(":"));
		// Get ID
		unsigned int user_id = std::stoul(split_inputs[0]);
		// Get Location
		std::string position_string = split_inputs[1];
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
		std::string answer = "a:" + this->GetTransmittersList();
		boost::asio::write(socket, boost::asio::buffer(answer), ign_err);
	} break;
	case '2': {
		// How many users are in the environment, who are they?
		std::cout << "Server: The client asks How many receivers?.\n";
		std::string answer = "a:" + this->GetReceiversList();
		boost::asio::write(socket, boost::asio::buffer(answer), ign_err);
	}break;
	case '3': {
		// Give me info of the station id #..
		std::cout << "Server: The client asks about a transmitter.\n";
		unsigned int id = std::stoi(question.substr(2));
		std::string answer = "a:" + this->GetTransmitterInfo(id);
		boost::asio::write(socket, boost::asio::buffer(answer), ign_err);
	} break;
	case '4': {
		// Give me information of the user number #..
		std::cout << "Server: The client asks about a user.\n";
		unsigned int id = std::stoi(question.substr(2));
		std::string answer = "a:" + this->GetReceiverInfo(id);
		boost::asio::write(socket, boost::asio::buffer(answer), ign_err);
	}break;
	case'5': {
        // Give me the q_map of station moving around the nap
        std::cout << "Server: The client asks for the average path loss q_map.\n";
        auto input_data = question.substr(2);
        std::vector<std::string> split_data;
        boost::split(split_data, input_data, boost::is_any_of(","));

        unsigned int station_id = std::stoul(split_data[0]);
        unsigned int resolution = std::stoul(split_data[1]);

        float x_start, x_end, z_start, z_end;
        ray_tracer_->GetMapBorder(x_start, x_end, z_start, z_end);

        float x_step = (x_end - x_start) / (float) resolution;
        float z_step = (z_end - x_start) / (float) resolution;

        auto q_map = this->GetStationMap(station_id, x_step, z_step);

		// Store the map locally.
		{
			std::string file_name = std::to_string(resolution) + "res" +
				std::to_string(transmitters_[station_id]->GetReceivers().size()) + ".csv";
			std::ofstream output_file{ "../assets/" + file_name };
			if (output_file.is_open()) {
				for (auto& [position, avg_pl] : q_map) {
					output_file << position.first << ", "
						<< position.second << ", "
						<< std::scientific << avg_pl << "\n";
				}
				output_file.close();
			}
			else {
				std::cout << "Unable to write the file.\n";
			}
		}

        if (q_map.empty())
            boost::asio::write(socket, boost::asio::buffer("fai"), ign_err);
        else
            boost::asio::write(socket, boost::asio::buffer("suc"), ign_err);

		boost::array<char, 1024> data_buffer{};
		int len = socket.read_some(boost::asio::buffer(data_buffer), ign_err);
		std::string rec_data = std::string(data_buffer.begin(), data_buffer.begin() + len);
		if (rec_data != "ready") {
			std::cout << "Communication Error.\n";
			return;
		}
        // Send the head of information.

		for (auto& [position, avg_pl] : q_map) {
			std::stringstream data_stream;
			data_stream << position.first << ","
				<< position.second << ","
				<< std::scientific << avg_pl;
			boost::asio::write(socket, boost::asio::buffer(data_stream.str()), ign_err);
			len = socket.read_some(boost::asio::buffer(data_buffer), ign_err);
			rec_data = std::string(data_buffer.begin(), data_buffer.begin() + len);
			if (rec_data != "ok") {
				std::cout << "Communication Error.\n";
				return;
			}
		}

		boost::asio::write(socket, boost::asio::buffer("end"), ign_err);

        std::cout<< "Successfully transferred the map\n";
	}break;
	default: {
		std::cout << "Server: Unknown Question\n";
		boost::asio::write(socket, boost::asio::buffer("fai"), ign_err);
	}break;
	}
}

bool Engine::AddTransmitter(glm::vec3 position, glm::vec3 rotation, float frequency)
{
	if (ray_tracer_ == nullptr) return false;
	auto * transmitter = new Transmitter({position, glm::vec3(1.0f) ,rotation },
                                                frequency, 0 ,ray_tracer_);
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
    rx->ConnectATransmitter(tx);
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
	map_ = new PolygonMesh("../assets/obj/poznan.obj", default_shader_, window_ != nullptr);
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
	//std::cout << "Printing\n";
	//Printer printer{ray_tracer_};
	//printer.Print("../test.ppm", glm::vec3(0.0f, 8.0f, 0.0f), 2.5e9f, 1.5f );
	//printer.TestPrint("../test-head-map.ppm");
}

void Engine::TransmitterMode(float delta_time)
{
	GLFWwindow* window = window_->GetGLFWWindow();
	std::cout << "Transmitter Mode\n";
	std::cout << "Press A - Add a transmitter\n";
	std::cout << "Press S - Select a transmitter\n";
	std::cout << "Press L - Display transmitter IDs\n";
	std::cout << "Press M - Control a transmitter\n";
	std::cout << "Press V - Go back to View Mode";
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
	    if(glfwGetKey(window, GLFW_KEY_V) != GLFW_PRESS)
        engine_mode_ = EngineMode::kView;
    }

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if(glfwGetKey(window, GLFW_KEY_A) != GLFW_PRESS) {
        Transform tx_trans;
		std::cout << "Enter the transmitter's information.\n";
		std::cout << "x: ";
		std::cin >> tx_trans.position.x;
		std::cout << "y: ";
		std::cin >> tx_trans.position.y;
		std::cout << "z: ";
		std::cin >> tx_trans.position.z;
		float frequency;
		std::cout << "frequency[Hz](eg. 2.5e9): ";
		std::cin >> frequency;
		std::cout << "transmit power[dBm]: ";
		float power;
		std::cin >> power;
		Transmitter* tx = new Transmitter(tx_trans, frequency, power, ray_tracer_);
		transmitters_.insert({ tx->GetID(), tx });
		std::cout << "Transmitter #" << tx->GetID() << " added to the environment.\n";
	    }
	}

}

void Engine::ReceiverMode(float delta_time)
{
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
		break;
	case kTransmitter:
		TransmitterMode(delta_time);
		break;
	case kReceiver:
		ReceiverMode(delta_time);
		break;
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

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		engine_mode_ = EngineMode::kTransmitter;

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		engine_mode_ = EngineMode::kReceiver;
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
	main_camera_->camera_move_speed_ += (float)yoffset;
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
	auto* my_engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
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



void Engine::ComputeMap( const glm::vec3  tx_position, const float tx_frequency,
                       std::vector<glm::vec3> * rx_positions,
						std::map<std::pair<float, float>, float>& map) const {
    float avg_total_loss = 0.0f;
    int n_users = 0;

    for(auto & rx_position: *rx_positions){
		std::vector<Record> records;
		Result result;

        ray_tracer_->TraceMap(tx_position,  rx_position, records);
        if(ray_tracer_->CalculatePathLossMap(tx_position, tx_frequency,
                                             rx_position, records, result)){
            ++n_users;
            avg_total_loss += result.total_attenuation;
        }
    }
    // Summary.
    if(n_users == 0){
        // In the case of base station is inside the building.
		map.insert({ std::make_pair(tx_position.x, tx_position.z), -200.0f });
	}
	else {
		// average the total loss and store to the map.
		map.insert({ std::make_pair(tx_position.x, tx_position.z), avg_total_loss / (float)n_users });
    }
    //std::cout << "Complete Tracing at: " << glm::to_string(position) << std::endl;
}

std::map<std::pair<float, float>, float> Engine::GetStationMap(unsigned int station_id,
                                                              float x_step, float z_step) {
    std::map<std::pair<float, float>, float> q_map;
    if (transmitters_.find(station_id) == transmitters_.end()) return q_map;
    Transmitter * tx = transmitters_.find(station_id)->second;
    float tx_frequency = tx->GetFrequency();
    float tx_height = tx->GetTransform().position.y;

    /// Get the order for image.
    float x_start, z_start, x_end, z_end;
    ray_tracer_->GetMapBorder(x_start, x_end, z_start, z_end);
    auto * rx_positions = new std::vector<glm::vec3>;
    for(auto [id, rx]: tx->GetReceivers()) rx_positions->push_back(rx->GetPosition());

    std::vector<std::thread> threads;
    unsigned int threads_limit = std::thread::hardware_concurrency()*20;
    for(float x = x_start; x <= x_end; x+=x_step) {
        for (float z = z_start; z <= z_end; z += z_step) {
            const glm::vec3 position{x, tx_height, z};
            std::thread map_thread(&Engine::ComputeMap, this,
                                   position, tx_frequency ,rx_positions,
                                   std::ref(q_map));
            threads.push_back(std::move(map_thread));

            if(threads.size() == threads_limit){
                for(auto & thread:threads) thread.join();
                threads.clear();
            }
        }

    }
    // Join threads
    for(auto & thread:threads) thread.join();
    threads.clear();
    return q_map;
}

RayTracer *Engine::GetRayTracer() const {
    return ray_tracer_;
}

