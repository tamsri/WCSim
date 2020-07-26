#include<iostream>

#define GLM_FORCE_CUDA

#include "window.hpp"
#include "engine.hpp"

int main(){
    Window * window = new Window(800, 600);
    Engine * engine = new Engine(window);
    //Engine* engine = new Engine(window);

    engine->LoadComponents();
    window->AssignEngine(engine);
    window->Run();

    return 0;
}