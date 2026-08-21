#include "Scene.hpp"

#include "physics/Physics.hpp"

#include <glm/glm.hpp>

#include <cmath>
#include <utility>

namespace {
constexpr const char* sunTexture = "../src/textures/planets/2k_sun.png";

RigidBody makeStar(
    const char* name,
    double massSolar,
    double radiusAU,
    const glm::dvec3& positionAU,
    const glm::dvec3& velocityAUPerYear)
{
    RigidBody star(massSolar, radiusAU);
    star.name = name;
    star.position = positionAU;
    star.velocity = velocityAUPerYear;
    star.isStar = true;
    star.loadTexture(sunTexture);
    return star;
}
}

Scene Scene::createDefault()
{
    Scene scene;

    // A hierarchical triple: two close stars orbit their shared barycenter,
    // while that binary and the third star orbit the system barycenter.
    // All motion begins in the XY plane and total linear momentum is zero.
    constexpr double primaryMass = 1.0;
    constexpr double secondaryMass = 0.8;
    constexpr double outerMass = 0.6;
    constexpr double innerSeparationAU = 1.5;
    constexpr double outerSeparationAU = 7.0;

    constexpr double binaryMass = primaryMass + secondaryMass;
    constexpr double totalMass = binaryMass + outerMass;

    const double binaryBarycenterX = -outerSeparationAU * outerMass / totalMass;
    const double outerStarX = outerSeparationAU * binaryMass / totalMass;
    const double primaryOffsetX = -innerSeparationAU * secondaryMass / binaryMass;
    const double secondaryOffsetX = innerSeparationAU * primaryMass / binaryMass;

    const double innerAngularSpeed = std::sqrt(
        Physics::gravitationalConstant * binaryMass
        / (innerSeparationAU * innerSeparationAU * innerSeparationAU));
    const double outerAngularSpeed = std::sqrt(
        Physics::gravitationalConstant * totalMass
        / (outerSeparationAU * outerSeparationAU * outerSeparationAU));
    const double binaryBarycenterVelocityY = outerAngularSpeed * binaryBarycenterX;

    scene.sceneBodies.push_back(makeStar(
        "Aster A",
        primaryMass,
        0.00465047,
        glm::dvec3(binaryBarycenterX + primaryOffsetX, 0.0, 0.0),
        glm::dvec3(
            0.0,
            binaryBarycenterVelocityY + innerAngularSpeed * primaryOffsetX,
            0.0)));

    scene.sceneBodies.push_back(makeStar(
        "Aster B",
        secondaryMass,
        0.0042,
        glm::dvec3(binaryBarycenterX + secondaryOffsetX, 0.0, 0.0),
        glm::dvec3(
            0.0,
            binaryBarycenterVelocityY + innerAngularSpeed * secondaryOffsetX,
            0.0)));

    scene.sceneBodies.push_back(makeStar(
        "Aster C",
        outerMass,
        0.0038,
        glm::dvec3(outerStarX, 0.0, 0.0),
        glm::dvec3(0.0, outerAngularSpeed * outerStarX, 0.0)));

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

void Scene::addBody(RigidBody body)
{
    sceneBodies.push_back(std::move(body));
}

bool Scene::removeBody(std::size_t bodyIndex)
{
    if (bodyIndex >= sceneBodies.size())
        return false;

    sceneBodies.erase(sceneBodies.begin() + static_cast<std::ptrdiff_t>(bodyIndex));
    return true;
}

void Scene::reset()
{
    sceneBodies = initialBodies;
}

void Scene::captureInitialState()
{
    initialBodies = sceneBodies;
}
