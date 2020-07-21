#include<iostream>

#include "windows.hpp"

int main(){
    Window * window = new Window(800, 600);
    Engine * engine = new Engine(window);
    engine->LoadComponents();
    window->AssignEngine(engine);
    window->Run();
    delete engine;
    delete window;
    return 0;
}