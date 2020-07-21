#include "engine.hpp"

#include <iostream>

#include "windows.hpp"

unsigned int Engine::global_engine_id_ = 0;

Engine::Engine():window_(nullptr), engine_id_(++global_engine_id_) {
	std::cout << "Initalize Engine without window. (ID: " << engine_id_ << ")." << std::endl;
}
Engine::Engine(Window* window) :window_(window), engine_id_(++global_engine_id_) {
	std::cout << "Initialize Engine with window. (ID: " << engine_id_ << ")." << std::endl;
}

void Engine::AssignWindow(Window* window)
{
	window_ = window;
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
}

void Engine::LoadObjects()
{
	std::cout << "Engine:Loading objects" << std::endl;

}

void Engine::LoadShaders()
{
	std::cout << "Engine:Loading shaders" << std::endl;

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

void Engine::Simulate() {


}

void Engine::Destroy()
{
	delete window_;
}
