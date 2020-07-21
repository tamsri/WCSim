#ifndef WINDOW_H
#define WINDOW_H
#include "engine.hpp"

class GLFWwindow;
class Engine;

class Window{

public:
    Window(int width, int height);
    ~Window();
    void Run();
    void AssignEngine(Engine * engine);
    static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
private:

    GLFWwindow * glfw_window_;
    Engine * engine_;
    unsigned int width_;
    unsigned int height_;

};
#endif //!WINDOW_H