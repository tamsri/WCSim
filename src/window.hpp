#ifndef WINDOW_H
#define WINDOW_H

class GLFWwindow;
class Engine;

class Window{

public:

    Window(int width, int height);
    ~Window();
    void Run();
    void AssignEngine(Engine * engine);
    void RemoveEngine();
    static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
    
    unsigned int GetWindowWidth() const;
    unsigned int GetWindowHeight() const;
    GLFWwindow * GetGLFWWindow() const;

private:

    GLFWwindow * glfw_window_;
    Engine * engine_;
    unsigned int width_;
    unsigned int height_;

};
#endif //!WINDOW_H