#include "Scene.hpp"

#include <glm/glm.hpp>

namespace {
constexpr double legacyRenderUnitInAU = 0.25;
constexpr double secondsPerYear = 31557600.0;

double legacyDistanceToAU(double legacyDistance)
{
    return legacyDistance * legacyRenderUnitInAU;
}

glm::dvec3 legacyPositionToAU(const glm::dvec3& legacyPosition)
{
    return legacyPosition * legacyRenderUnitInAU;
}

glm::dvec3 legacyVelocityToAUPerYear(const glm::dvec3& legacyVelocityPerSecond)
{
    return legacyVelocityPerSecond * legacyRenderUnitInAU * secondsPerYear;
}
}

Scene Scene::createDefault()
{
    Scene scene;

    RigidBody star(1.0, legacyDistanceToAU(0.01860188));
    star.setPosition(legacyPositionToAU(glm::dvec3(0.0, 0.0, 0.0)));
    star.setVelocity(legacyVelocityToAUPerYear(glm::dvec3(0.0, 0.0, 0.0)));
    star.isStar = true;
    star.loadTexture("../src/textures/planets/2k_sun.png");
    scene.sceneBodies.push_back(star);

    RigidBody companion(0.33, legacyDistanceToAU(0.001860188));
    companion.setPosition(legacyPositionToAU(glm::dvec3(4.0, 0.0, 0.0)));
    companion.setVelocity(legacyVelocityToAUPerYear(
        glm::dvec3(0.0, 0.0, 4.0 * 2.0053761e-7)));
    companion.loadTexture("../src/textures/planets/2k_earth_daymap.png");
    scene.sceneBodies.push_back(companion);

    scene.captureInitialState();
    return scene;
}

std::vector<RigidBody>& Scene::bodies()
{
    return sceneBodies;
}

const std::vector<RigidBody>& Scene::bodies() const
{
    return sceneBodies;
}

void Scene::reset()
{
    sceneBodies = initialBodies;
}

void Scene::captureInitialState()
{
    initialBodies = sceneBodies;
}
