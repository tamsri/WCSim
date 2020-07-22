#ifndef ENGINE_H
#define ENGINE_H

#include "glm/glm.hpp"

class Window;
class Camera;


class Engine{
    public:
        Engine();
        Engine(Window* window);
        void AssignWindow(Window * window);
        void Destroy();
        // Visualization
        void LoadComponents();
        void LoadMap();
        void LoadObjects();
        void LoadShaders();
        void LoadTexture();
        void Visualize();
        // Ray Tracer
        void InitializeRays();
        void InitializeVoxels();
        void TracePathFrom(glm::vec3 position);
        void Update();

    private:
        // Engine Parameters
        unsigned int engine_id_;
        Window * window_;
        static unsigned int global_engine_id_;
        
        // Engine Simulation


        // Engine Visualisation
        Camera * main_camera_;


        // Control from Windows

        // Control from Communication Pipe
};

#endif // !ENGINE_H
