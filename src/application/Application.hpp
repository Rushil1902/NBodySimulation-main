#pragma once

#include <memory>

struct GLFWwindow;
class Renderer;
class Scene;
class Simulation;

class Application {
public:
    Application();
    ~Application();

    int run();

private:
    bool initialize();
    void shutdown();
    void processInput();
    void updateWindowTitle(double currentTime);
    bool keyPressedOnce(int key, bool& wasPressed) const;

    static constexpr int initialWidth = 1200;
    static constexpr int initialHeight = 675;

    GLFWwindow* window = nullptr;
    bool glfwInitialized = false;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Scene> scene;
    std::unique_ptr<Simulation> simulation;
    double previousFrameTime = 0.0;
    double lastTitleUpdateTime = 0.0;
    bool spaceWasPressed = false;
    bool resetWasPressed = false;
    bool stepWasPressed = false;
    bool fasterWasPressed = false;
    bool slowerWasPressed = false;
};
