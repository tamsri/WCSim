#ifndef ENGINE_H
#define ENGINE_H

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Window;
class Camera;
class Shader;
class GLFWwindow;
class PolygonMesh;
class Cube;
class Ray;
class Object;
class Transmitter;
class RayTracer;
class Receiver;
class RadiationPattern;
class Communicator;
class Recorder;

enum EngineMode : int {
    kView = 0,
    kMoveObjects,
    kSimulation,
    kServer
};

enum MouseBottons : int {
    kRightPress = 0,
    kRightRelease,
    kLeftPress,
    kLeftRelease
};

class Engine{
    public:
        Engine();
        Engine(Window* window);
        ~Engine();
        void AssignWindow(Window * window);

        // Main Process
        void InitializeWithoutWindow();
        void Reset();
        void Run();
        void RunWithWindow();

        // Exernal Actions
        unsigned int GetTransmittersNumber() const;
        unsigned int GetReceiversNumber() const;
        bool AddTransmitter(glm::vec3 position, glm::vec3 rotation, float frequency);
        bool AddReceiver(glm::vec3 position);
        bool AddTransmitterToReceiver(unsigned int tx_id, unsigned int rx_id);

        // Visualization
        void InitalizeWithWindow();
        void LoadComponents();
        void LoadMap();
        void LoadObjects();
        void LoadShaders();
        void LoadTexture();
        void Visualize();
        void OnKeys();

        // Ray Tracer
        void LoadRayTracer();
        void Update();
        void PrintMap();

    private:
        // Engine Parameters
        unsigned int engine_id_;
        EngineMode engine_mode_;
        Window * window_;
        static unsigned int global_engine_id_;
        
        // Engine Simulation
        RayTracer* ray_tracer_;
        Shader * default_shader_;
        PolygonMesh * map_;

        
        Recorder* recorder_;

        std::vector<Receiver *> receivers_;
        std::vector<Transmitter *> transmitters_;
        std::vector<RadiationPattern> pattern_;


        float last_time_;


        void KeyActions();
        void KeyViewMode(float delta_time);
        void KeyMoveMode(float delta_time);
        void MousePosition(double xpos, double ypos);
        void MouseScroll(double xoffset, double yoffset);
        void MouseBottonToggler(MouseBottons action);

        bool on_first_right_click_;
        bool on_first_left_click_;

        bool on_right_click_;
        bool on_left_click_;

        double last_mouse_x_pos_;
        double last_mouse_y_pos_;
        static void MousePositionCallback(GLFWwindow* window, double xpos, double ypos);
        static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        // Engine Visualisation
        Camera * main_camera_;

        // Control from Windows

        // Control from Communication Pipe

        // Communication
        Communicator* communicator_;
};

#endif // !ENGINE_H
