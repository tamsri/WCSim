#ifndef ENGINE_H
#define ENGINE_H

class Window;

class Engine{
    public:
        Engine(Window* window);

        // Visualization
        void LoadShapes();
        void LoadShaders();
        void LoadTexture();

        // Ray Tracer
        void InitializeRays();
        void InitializeVoxels();

        void Simulate();
        void Destroy();
    private:
        Window * window_;
};
#endif // !ENGINE_H
