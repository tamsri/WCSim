#include "window.hpp"

#include<cassert>
#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include "engine.hpp"



Window::Window(int width, int height) : width_(width), height_(height) {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    glfw_window_ = glfwCreateWindow(
        width_,
        height_,
        "WCSim",
        NULL,
        NULL);

    if (glfw_window_ == NULL)
    {
        assert("Failed to create GLFW window");
        glfwTerminate();
    }

    glfwMakeContextCurrent(glfw_window_);
    glfwSetFramebufferSizeCallback(glfw_window_, FrameBufferSizeCallback);
    glfwSetWindowUserPointer(glfw_window_, this);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
}
void Window::FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    Window* p_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    glViewport(0, 0, width, height);
    p_window->Resize(width, height);
}
unsigned int Window::GetWindowWidth() const
{
    return width_;
}
unsigned int Window::GetWindowHeight() const
{
    return height_;
}
GLFWwindow* Window::GetGLFWWindow() const
{
    return glfw_window_;
}
void Window::Resize(int width, int height)
{
    width_ = width;
    height_ = height;
}

Window::~Window() {
    //delete glfw_window_; // TODO: check if can be released.
    glfwTerminate();
}

void Window::Run() {
    if (glfw_window_ == NULL)
    {
        assert("cannot open the window");
        glfwTerminate();
        return;
    }
    if (engine_ == nullptr) return;

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);
    while (!glfwWindowShouldClose(glfw_window_))
    {
        /*Render from Engine*/
        glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        UpdateVisualComponents();
        VisualizeComponents();

        OnKeysPressed();

        /*Swap frames*/
        glfwSwapBuffers(glfw_window_);
        glfwPollEvents();
    }
    glfwTerminate();
}
void Window::AssignEngine(Engine* engine)
{
    engine_ = engine;
}

void Window::RemoveEngine()
{
    engine_ = nullptr;
}

void Window::UpdateVisualComponents() {
    engine_->UpdateVisualComponents();
}

void Window::VisualizeComponents() {
    engine_->Visualize();
}

void Window::OnKeysPressed() {
    engine_->OnKeys();
}
