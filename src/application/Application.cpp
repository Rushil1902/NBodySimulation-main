#include "Application.hpp"

#include "core/AssetPaths.hpp"
#include "rendering/Renderer.hpp"
#include "scene/Scene.hpp"
#include "simulation/Simulation.hpp"
#include "ui/UserInterface.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <iostream>
#include <sstream>

Application::Application() = default;

Application::~Application()
{
    shutdown();
}

int Application::run()
{
    if (!initialize())
        return -1;

    std::cout << "Current working directory:\n"
              << std::filesystem::current_path() << '\n'
              << "Runtime asset directory:\n"
              << AssetPaths::root() << '\n'
              << "Simulation controls: Space = pause/resume, N = single step, "
              << "R = reset, -/= = slower/faster\n";

    previousFrameTime = glfwGetTime();
    lastTitleUpdateTime = previousFrameTime;

    while (!glfwWindowShouldClose(window))
    {
        renderer->setCameraInputEnabled(!userInterface->wantsMouseCapture());
        glfwPollEvents();
        userInterface->beginFrame();
        renderer->setCameraInputEnabled(!userInterface->wantsMouseCapture());
        processInput();
        userInterface->draw(*scene, *simulation, *renderer);

        const double currentTime = glfwGetTime();
        const double realFrameTime = currentTime - previousFrameTime;
        previousFrameTime = currentTime;

        const bool physicsAdvanced = simulation->update(realFrameTime);
        renderer->render(*scene, physicsAdvanced, currentTime);
        userInterface->render();
        updateWindowTitle(currentTime);

        glfwSwapBuffers(window);
    }

    shutdown();
    return 0;
}

bool Application::initialize()
{
    if (!glfwInit())
        return false;
    glfwInitialized = true;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(
        initialWidth,
        initialHeight,
        "N-Body Simulation",
        nullptr,
        nullptr);
    if (!window)
        return false;

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    glfwSwapInterval(1);

    try
    {
        renderer = std::make_unique<Renderer>();
        renderer->attachToWindow(window);
        scene = std::make_unique<Scene>(Scene::createDefault());
        simulation = std::make_unique<Simulation>(*scene);
        userInterface = std::make_unique<UserInterface>(window);
    }
    catch (const std::exception& error)
    {
        std::cerr << "Application initialization failed: " << error.what() << '\n';
        return false;
    }
    return true;
}

void Application::shutdown()
{
    userInterface.reset();
    simulation.reset();
    scene.reset();
    renderer.reset();

    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    if (glfwInitialized)
    {
        glfwTerminate();
        glfwInitialized = false;
    }
}

void Application::processInput()
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (userInterface && userInterface->wantsKeyboardCapture())
        return;

    if (keyPressedOnce(GLFW_KEY_SPACE, spaceWasPressed))
        simulation->togglePaused();

    if (keyPressedOnce(GLFW_KEY_R, resetWasPressed))
        simulation->reset();

    if (keyPressedOnce(GLFW_KEY_N, stepWasPressed))
        simulation->requestSingleStep();

    if (keyPressedOnce(GLFW_KEY_EQUAL, fasterWasPressed))
        simulation->increaseSpeed();

    if (keyPressedOnce(GLFW_KEY_MINUS, slowerWasPressed))
        simulation->decreaseSpeed();
}

void Application::updateWindowTitle(double currentTime)
{
    if (currentTime - lastTitleUpdateTime < 0.25)
        return;

    std::ostringstream title;
    title.precision(4);
    title << "N-Body Simulation | "
          << (simulation->isPaused() ? "Paused" : "Running")
          << " | t=" << simulation->elapsedYears() << " yr"
          << " | rate=" << simulation->yearsPerRealSecond() << " yr/s"
          << " | energy drift=" << std::scientific << simulation->relativeEnergyDrift();
    glfwSetWindowTitle(window, title.str().c_str());
    lastTitleUpdateTime = currentTime;
}

bool Application::keyPressedOnce(int key, bool& wasPressed) const
{
    const bool isPressed = glfwGetKey(window, key) == GLFW_PRESS;
    const bool pressedOnce = isPressed && !wasPressed;
    wasPressed = isPressed;
    return pressedOnce;
}
