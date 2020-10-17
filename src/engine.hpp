#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <string>
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

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
namespace ip = boost::asio::ip;


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
        std::string GetTransmittersList() const;
        std::string GetTransmitterInfo(unsigned int transmitter_id);
        std::string GetReceiversList() const;
        std::string GetReceiverInfo(unsigned int receiver_id);

        // Main orders
        void ExecuteCommand(ip::tcp::socket& socket, boost::system::error_code& ign_err, std::string& command);
        void ExecuteQuestion(ip::tcp::socket& socket, boost::system::error_code& ign_err, std::string& question);

        // Commands for TCP 
        bool AddTransmitter(glm::vec3 position, glm::vec3 rotation, float frequency);
        bool AddReceiver(glm::vec3 position);
        bool RemoveTransmitter(unsigned int transmitter_id);
        bool RemoveReceiver(unsigned int receiver_id);
        bool ConnectReceiverToTransmitter(unsigned int tx_id, unsigned int rx_id);
        bool DisconnectReceiverFromTransmitter(unsigned int tx_id, unsigned int rx_id);
        bool MoveTransmitterTo(unsigned int rx_id, glm::vec3 position, glm::vec3 rotation);
        bool MoveReceiverTo(unsigned int rx_id, glm::vec3 position);

        // Visualization
        void InitializeWithWindow();
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

        std::map<unsigned int ,Receiver *> receivers_;
        std::map<unsigned int, Transmitter *> transmitters_;
        std::vector<RadiationPattern> pattern_;


        float last_time_;


        void KeyActions();
        void KeyViewMode(float delta_time);
        void KeyMoveMode(float delta_time);
        void MousePosition(double x_pos, double ypos);
        void MouseScroll(double xoffset, double yoffset);
        void MouseButtonToggle(MouseBottons action);

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
