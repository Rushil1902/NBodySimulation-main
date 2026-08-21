#pragma once

struct GLFWwindow;
class Renderer;
class Scene;
class Simulation;

class UserInterface {
public:
    explicit UserInterface(GLFWwindow* window);
    ~UserInterface();

    void beginFrame();
    void draw(Scene& scene, Simulation& simulation, Renderer& renderer);
    void render();

    bool wantsKeyboardCapture() const;
    bool wantsMouseCapture() const;

private:
    int selectedBody = 0;
    bool followSelectedBody = false;
    bool followSelectedSlice = false;
    bool initialized = false;
};
