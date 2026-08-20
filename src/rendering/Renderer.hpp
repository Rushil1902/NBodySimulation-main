#pragma once

#include "core/Mesh.hpp"
#include "core/SkyboxMesh.hpp"
#include "core/TrailMesh.hpp"
#include "geometry/Transform.hpp"
#include "graphics/camera.hpp"
#include "graphics/shader.h"
#include "physics/SpaceTimeGrid.hpp"

struct GLFWwindow;
class Scene;

class Renderer {
public:
    Renderer();

    void attachToWindow(GLFWwindow* window);
    void render(Scene& scene, bool physicsAdvanced, double rotationTimeSeconds);
    void resize(int width, int height);
    void setGridVisible(bool visible);
    bool isGridVisible() const;

private:
    static Renderer* fromWindow(GLFWwindow* window);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

    Shader cameraShader;
    Shader skyboxShader;
    Shader pathShader;
    Shader planetShader;
    Shader starShader;
    Shader gridShader;
    SkyboxMesh skybox;
    Transform starTransforms;
    Transform planetTransforms;
    Mesh sphereMesh;
    TrailMesh trailMesh;
    SpaceTimeGrid grid;
    Camera camera;
    bool gridVisible = false;
};
