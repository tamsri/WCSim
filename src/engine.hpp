#ifndef ENGINE_H
#define ENGINE_H

class Window;

class Engine{
    public:
        Engine();
        Engine(Window* window);
        void AssignWindow(Window * window);
        // Visualization
        void LoadComponents();
        void LoadMap();
        void LoadObjects();
        void LoadShaders();
        void LoadTexture();

        // Ray Tracer
        void InitializeRays();
        void InitializeVoxels();

        void Simulate();
        void Destroy();
    private:
        unsigned int engine_id_;
        Window * window_;
        static unsigned int global_engine_id_;
};

#endif // !ENGINE_H
