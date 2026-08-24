#include "Renderer.hpp"

#include "geometry/ShapeGenerator.hpp"
#include "rendering/RenderScale.hpp"
#include "scene/Scene.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

namespace {
constexpr float initialFieldOfView = 45.0f;
constexpr float nearPlane = 0.1f;
constexpr float farPlane = 200.0f;
}

Renderer::Renderer()
    : cameraShader("../src/graphics/meshShaders/3.3.shader.vs",
                   "../src/graphics/meshShaders/3.3.shader.fs"),
      skyboxShader("../src/graphics/SkyboxShaders/SkyboxShader.vs",
                   "../src/graphics/SkyboxShaders/SkyboxShader.fs"),
      pathShader("../src/graphics/TrailShaders/TrailShader.vs",
                 "../src/graphics/TrailShaders/TrailShader.fs"),
      planetShader("../src/graphics/PlanetShaders/PlanetShader.vs",
                   "../src/graphics/PlanetShaders/PlanetShader.fs"),
      starShader("../src/graphics/StarShaders/StarShader.vs",
                 "../src/graphics/StarShaders/StarShader.fs"),
      coronaShader("../src/graphics/StarShaders/CoronaShader.vs",
                   "../src/graphics/StarShaders/CoronaShader.fs"),
      gridShader("../src/graphics/GridShaders/GridShader.vs",
                 "../src/graphics/GridShaders/GridShader.fs"),
      starTransforms(starShader),
      coronaTransforms(coronaShader),
      planetTransforms(planetShader),
      sphereMesh(ShapeGenerator::makeUnitSphere(128, 128)),
      grid(64),
      camera(cameraShader)
{
    const std::vector<std::string> faces = {
        "../src/textures/skybox/GalaxyTex_PositiveX.png",
        "../src/textures/skybox/GalaxyTex_NegativeX.png",
        "../src/textures/skybox/GalaxyTex_PositiveY.png",
        "../src/textures/skybox/GalaxyTex_NegativeY.png",
        "../src/textures/skybox/GalaxyTex_PositiveZ.png",
        "../src/textures/skybox/GalaxyTex_NegativeZ.png"
    };
    skybox.loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    cameraShader.use();
    camera.setPerspective(
        glm::radians(initialFieldOfView),
        1200.0f / 675.0f,
        nearPlane,
        farPlane);
    camera.setCameraPosition(glm::vec3(0.0f, 0.0f, -50.0f));
    camera.setOrbitAngles(0.0f, glm::radians(-25.0f));
}

void Renderer::attachToWindow(GLFWwindow* window)
{
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    resize(width, height);
}

void Renderer::render(Scene& scene, bool physicsAdvanced, double rotationTimeSeconds)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cameraShader.use();
    camera.updateCameraPosition();

    starShader.use();
    starShader.setMat4("view", camera.getViewMatrix());
    starShader.setMat4("projection", camera.getPerspectiveMatrix());
    starShader.setVec3("viewPos", camera.position);
    starShader.setFloat("timeSeconds", static_cast<float>(rotationTimeSeconds));

    for (RigidBody& body : scene.bodies())
    {
        if (!body.isStar)
            continue;

        starShader.setFloat("starSeed", static_cast<float>(body.starSeed));
        starShader.setFloat("cellScale", body.starCellScale);
        starShader.setFloat("activity", body.starActivity);
        starShader.setFloat("animationSpeed", body.starAnimationSpeed);
        starShader.setFloat("spotAmount", body.starSpotAmount);
        starShader.setVec3("starColorA", body.starColorA);
        starShader.setVec3("starColorB", body.starColorB);
        starShader.setVec3("starColorC", body.starColorC);
        starShader.setVec3("starColorD", body.starColorD);
        starShader.setVec3("starColorE", body.starColorE);
        starShader.setVec3("starColorF", body.starColorF);
        starShader.setVec3("starColorG", body.starColorG);

        const float starRadius = RenderScale::radius(body.bodyRadius)
            * std::max(body.displayScale, 0.01f);
        starTransforms.setScale(glm::vec3(starRadius));
        starTransforms.setTranslate(RenderScale::position(body.position));
        starTransforms.setRotate(
            static_cast<float>(12.0 * rotationTimeSeconds * body.starAnimationSpeed),
            glm::vec3(0.0f, 0.0f, 1.0f));
        starTransforms.applyTransformation();
        sphereMesh.draw();
    }

    planetShader.use();
    planetShader.setMat4("view", camera.getViewMatrix());
    planetShader.setMat4("projection", camera.getPerspectiveMatrix());
    planetShader.setVec3("viewPos", camera.position);
    planetShader.setFloat("timeSeconds", static_cast<float>(rotationTimeSeconds));
    const RigidBody* primaryStar = nullptr;
    for (const RigidBody& body : scene.bodies())
    {
        if (body.isStar)
        {
            primaryStar = &body;
            break;
        }
    }
    if (primaryStar != nullptr)
    {
        planetShader.setVec3("lightPos", RenderScale::position(primaryStar->position));
        planetShader.setVec3(
            "lightColor",
            glm::mix(glm::vec3(1.0f), primaryStar->starColorE, 0.22f));
    }
    else
    {
        planetShader.setVec3("lightPos", glm::vec3(0.0f));
        planetShader.setVec3("lightColor", glm::vec3(1.0f));
    }

    for (RigidBody& body : scene.bodies())
    {
        if (body.isStar)
            continue;

        planetShader.setInt("surfaceType", static_cast<int>(body.surfaceType));
        planetShader.setFloat("proceduralSeed", static_cast<float>(body.proceduralSeed));
        planetShader.setFloat("terrainScale", body.terrainScale);
        planetShader.setFloat("oceanLevel", body.oceanLevel);
        planetShader.setFloat("cloudCoverage", body.cloudCoverage);
        planetShader.setFloat("animationSpeed", body.surfaceAnimationSpeed);
        planetShader.setVec3("surfaceColorA", body.surfaceColorA);
        planetShader.setVec3("surfaceColorB", body.surfaceColorB);
        planetShader.setVec3("surfaceColorC", body.surfaceColorC);
        planetShader.setVec3("surfaceColorD", body.surfaceColorD);
        planetShader.setVec3("surfaceColorE", body.surfaceColorE);
        planetShader.setVec3("surfaceColorF", body.surfaceColorF);
        planetShader.setVec3("surfaceColorG", body.surfaceColorG);
        planetShader.setFloat("surfaceVibrancy", body.surfaceVibrancy);
        planetShader.setFloat("surfaceColorBanding", body.surfaceColorBanding);
        if (body.surfaceType == PlanetSurfaceType::Texture)
            body.bindTexture();

        planetTransforms.setScale(glm::vec3(
            RenderScale::radius(body.bodyRadius) * std::max(body.displayScale, 0.01f)));
        planetTransforms.setTranslate(RenderScale::position(body.position));
        if (body.surfaceType == PlanetSurfaceType::Texture)
            planetTransforms.setRotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        planetTransforms.setRotate(
            static_cast<float>(20.0 * rotationTimeSeconds * body.surfaceAnimationSpeed),
            glm::vec3(0.0f, 0.0f, 1.0f));
        planetTransforms.applyTransformation();
        sphereMesh.draw();
    }

    coronaShader.use();
    coronaShader.setMat4("view", camera.getViewMatrix());
    coronaShader.setMat4("projection", camera.getPerspectiveMatrix());
    coronaShader.setVec3("viewPos", camera.position);
    coronaShader.setFloat("timeSeconds", static_cast<float>(rotationTimeSeconds));

    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (RigidBody& body : scene.bodies())
    {
        if (!body.isStar || body.starCoronaIntensity <= 0.0f)
            continue;

        coronaShader.setFloat("starSeed", static_cast<float>(body.starSeed));
        coronaShader.setFloat("activity", body.starActivity);
        coronaShader.setFloat("animationSpeed", body.starAnimationSpeed);
        coronaShader.setFloat("coronaIntensity", body.starCoronaIntensity);
        coronaShader.setVec3("coronaColorInner", body.starColorF);
        coronaShader.setVec3("coronaColorOuter", body.starColorG);

        const float starRadius = RenderScale::radius(body.bodyRadius)
            * std::max(body.displayScale, 0.01f);
        const float coronaRadius = starRadius * (1.0f + body.starCoronaSize);
        coronaTransforms.setScale(glm::vec3(coronaRadius));
        coronaTransforms.setTranslate(RenderScale::position(body.position));
        coronaTransforms.setRotate(
            static_cast<float>(-8.0 * rotationTimeSeconds * body.starAnimationSpeed),
            glm::vec3(0.0f, 0.0f, 1.0f));
        coronaTransforms.applyTransformation();
        sphereMesh.draw();
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);

    if (trailsVisible)
    {
        pathShader.use();
        pathShader.setMat4("view", camera.getViewMatrix());
        pathShader.setMat4("projection", camera.getPerspectiveMatrix());
        for (RigidBody& body : scene.bodies())
        {
            if (physicsAdvanced)
                body.updatePath(RenderScale::position(body.position));

            trailMesh.updateTrail(body.path, body.pathAlphas);
            trailMesh.draw();
        }
    }

    glDepthFunc(GL_LEQUAL);
    skyboxShader.use();
    skyboxShader.setMat4("view", camera.getViewMatrix());
    skyboxShader.setMat4("projection", camera.getPerspectiveMatrix());
    skybox.draw();
    glDepthFunc(GL_LESS);

    if (gridVisible)
    {
        gridShader.use();
        gridShader.setMat4("view", camera.getViewMatrix());
        gridShader.setMat4("projection", camera.getPerspectiveMatrix());
        gridShader.setFloat("surfaceOpacity", fieldSettings.surfaceOpacity);
        grid.update(scene.bodies(), fieldSettings);

        if (fieldSettings.surfaceVisible)
        {
            gridShader.setBool("surfacePass", true);
            glDepthMask(GL_FALSE);
            grid.drawSurface();
            glDepthMask(GL_TRUE);
        }

        gridShader.setBool("surfacePass", false);
        grid.drawLines();
    }
}

void Renderer::resize(int width, int height)
{
    if (width <= 0 || height <= 0)
        return;

    glViewport(0, 0, width, height);
    cameraShader.use();
    camera.setPerspective(
        glm::radians(initialFieldOfView),
        static_cast<float>(width) / static_cast<float>(height),
        nearPlane,
        farPlane);
}

void Renderer::setGridVisible(bool visible)
{
    gridVisible = visible;
}

bool Renderer::isGridVisible() const
{
    return gridVisible;
}

GravityFieldSettings& Renderer::gravityFieldSettings()
{
    return fieldSettings;
}

const GravityFieldSettings& Renderer::gravityFieldSettings() const
{
    return fieldSettings;
}

void Renderer::setTrailsVisible(bool visible)
{
    trailsVisible = visible;
}

bool Renderer::areTrailsVisible() const
{
    return trailsVisible;
}

void Renderer::focusOnBody(const RigidBody& body)
{
    camera.setFocusPoint(RenderScale::position(body.position));
}

void Renderer::resetCameraFocus()
{
    camera.resetFocusPoint();
}

void Renderer::setCameraInputEnabled(bool enabled)
{
    cameraInputEnabled = enabled;
}

Renderer* Renderer::fromWindow(GLFWwindow* window)
{
    return static_cast<Renderer*>(glfwGetWindowUserPointer(window));
}

void Renderer::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    if (Renderer* renderer = fromWindow(window))
        renderer->resize(width, height);
}

void Renderer::scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    if (Renderer* renderer = fromWindow(window); renderer && renderer->cameraInputEnabled)
        renderer->camera.scroll_callback(xOffset, yOffset);
}

void Renderer::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (Renderer* renderer = fromWindow(window);
        renderer && (renderer->cameraInputEnabled || action == GLFW_RELEASE))
        renderer->camera.mouse_button_callback(window, button, action, mods);
}

void Renderer::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (Renderer* renderer = fromWindow(window); renderer && renderer->cameraInputEnabled)
        renderer->camera.cursor_position_callback(xpos, ypos);
}
