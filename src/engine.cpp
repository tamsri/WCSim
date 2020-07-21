#include "engine.hpp"

Engine::Engine(Window* window) :window_(window) {

}

void Engine::Simulate() {
	


}

void Engine::Destroy()
{
	delete window_;
}
