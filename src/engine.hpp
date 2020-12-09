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
class ConsoleController;

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>

namespace ip = boost::asio::ip;


enum EngineMode : int {
    kView = 0,
    kTransmitter,
    kReceiver
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

        // Main Process
        void InitializeWithoutWindow();
        void Reset();

        void RunWithWindow();

        // Main TC orders
        void ExecuteCommand(ip::tcp::socket& socket, boost::system::error_code& ign_err, std::string& command);
        void ExecuteQuestion(ip::tcp::socket& socket, boost::system::error_code& ign_err, std::string& question);


        // External Actions
        std::string GetTransmittersList() const;
        std::string GetTransmitterInfo(unsigned int transmitter_id);
        std::string GetReceiversList() const;
        std::string GetReceiverInfo(unsigned int receiver_id);
        std::map<std::pair<float, float>, float>  GetStationMap(unsigned int station_id, float x_step, float z_step);
        void ComputeMap(glm::vec3 tx_positions, float tx_frequency,
                        std::vector<glm::vec3> * rx_positions,
                        std::map<std::pair<float, float>, float> & map) const;
        std::string GetPossiblePath(glm::vec3 start_position, glm::vec3 end_position) const;



        // Commands for TCP 
        bool AddTransmitter(glm::vec3 position, glm::vec3 rotation, float frequency);
        bool AddReceiver(glm::vec3 position);
        bool RemoveTransmitter(unsigned int transmitter_id);
        bool RemoveReceiver(unsigned int receiver_id);
        bool ConnectReceiverToTransmitter(unsigned int tx_id, unsigned int rx_id);
        bool DisconnectReceiverFromTransmitter(unsigned int tx_id, unsigned int rx_id);
        bool MoveTransmitterTo(unsigned int rx_id, glm::vec3 position, glm::vec3 rotation);
        bool MoveReceiverTo(unsigned int rx_id, glm::vec3 position);
        bool IsDirect(glm::vec3 start_position, glm::vec3 end_position);
        bool IsOutdoor(glm::vec3 position);
        bool UpdateResults();
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
        RayTracer * GetRayTracer() const;
        void LoadRayTracer();
        void Update();

        Transmitter * current_transmitter_;
        Receiver * current_receiver_;
        std::map<unsigned int , Receiver *> receivers_;
        std::map<unsigned int, Transmitter *> transmitters_;
        EngineMode engine_mode_;

        std::vector<Receiver *> updated_receivers_;
        std::vector<Transmitter *> updated_transmitters_;

        void UpdateVisualComponents();
        Shader* default_shader_;

        bool IsWindowOn();
private:
        // Engine Parameters
        unsigned int engine_id_;
        Window * window_;
        static unsigned int global_engine_id_;
        
        // Engine Simulation
        RayTracer* ray_tracer_;
        PolygonMesh * map_;

        
        Recorder* recorder_;

        std::vector<RadiationPattern *> patterns_;


        float last_time_;

        void TransmitterMode(float delta_time);
        void ReceiverMode(float delta_time);
        void KeyActions();
        void KeyViewMode(float delta_time);
        void KeyRXMoveMode(float delta_time);
        void KeyTXMoveMode(float delta_time);
        void MousePosition(double x_pos, double ypos);
        void MouseScroll(double xoffset, double yoffset);
        void MouseButtonToggle(MouseBottons action);

        bool on_pressed_;

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
};

#endif // !ENGINE_H
