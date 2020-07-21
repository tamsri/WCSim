#include<iostream>

#include "windows.hpp"

int main(){
    Window * window = new Window(800, 600);
    window->Run();
    delete window;
    return 0;
}