#include "Scene.hpp"

#include "physics/Physics.hpp"

#include <glm/glm.hpp>

#include <cmath>
#include <utility>

namespace {
constexpr double pi = 3.14159265358979323846;

RigidBody makeProceduralPlanet(
    const char* name,
    double massSolar,
    double radiusAU,
    double orbitRadiusAU,
    double orbitAngle,
    PlanetSurfaceType surfaceType,
    int seed,
    float displayScale,
    float terrainScale)
{
    const double orbitalSpeed = std::sqrt(
        Physics::gravitationalConstant * (1.0 + massSolar) / orbitRadiusAU);
    const glm::dvec3 radialDirection(
        std::cos(orbitAngle),
        std::sin(orbitAngle),
        0.0);
    const glm::dvec3 tangentDirection(
        -std::sin(orbitAngle),
        std::cos(orbitAngle),
        0.0);

    RigidBody planet(massSolar, radiusAU);
    planet.name = name;
    planet.position = radialDirection * orbitRadiusAU;
    planet.velocity = tangentDirection * orbitalSpeed;
    planet.surfaceType = surfaceType;
    planet.proceduralSeed = seed;
    planet.displayScale = displayScale;
    planet.terrainScale = terrainScale;
    return planet;
}
}

Scene Scene::createDefault()
{
    Scene scene;

    std::vector<RigidBody> planets;

    RigidBody earthLike = makeProceduralPlanet(
        "Procedural Terran",
        3.0034896e-6,
        4.26352e-5,
        1.5,
        0.0,
        PlanetSurfaceType::EarthLike,
        101,
        300.0f,
        3.6f);
    earthLike.oceanLevel = 0.50f;
    earthLike.cloudCoverage = 0.42f;
    earthLike.surfaceAnimationSpeed = 0.65f;
    earthLike.surfaceColorA = glm::vec3(0.015f, 0.07f, 0.18f);
    earthLike.surfaceColorB = glm::vec3(0.0f, 0.55f, 0.75f);
    earthLike.surfaceColorC = glm::vec3(0.03f, 0.24f, 0.10f);
    earthLike.surfaceColorD = glm::vec3(0.48f, 0.82f, 0.08f);
    earthLike.surfaceColorE = glm::vec3(0.64f, 0.38f, 0.82f);
    earthLike.surfaceColorF = glm::vec3(0.88f, 0.94f, 1.0f);
    earthLike.surfaceColorG = glm::vec3(0.98f, 0.99f, 1.0f);
    planets.push_back(std::move(earthLike));

    RigidBody gasGiant = makeProceduralPlanet(
        "Procedural Gas Giant",
        9.54588e-4,
        0.000477895,
        4.0,
        2.0 * pi / 3.0,
        PlanetSurfaceType::GasGiant,
        202,
        35.0f,
        4.8f);
    gasGiant.surfaceAnimationSpeed = 0.45f;
    gasGiant.surfaceColorA = glm::vec3(0.08f, 0.015f, 0.18f);
    gasGiant.surfaceColorB = glm::vec3(0.28f, 0.05f, 0.55f);
    gasGiant.surfaceColorC = glm::vec3(0.65f, 0.08f, 0.72f);
    gasGiant.surfaceColorD = glm::vec3(0.95f, 0.20f, 0.52f);
    gasGiant.surfaceColorE = glm::vec3(1.0f, 0.55f, 0.18f);
    gasGiant.surfaceColorF = glm::vec3(0.02f, 0.75f, 0.92f);
    gasGiant.surfaceColorG = glm::vec3(0.80f, 0.98f, 1.0f);
    planets.push_back(std::move(gasGiant));

    RigidBody lavaWorld = makeProceduralPlanet(
        "Procedural Lava World",
        1.5e-5,
        6.0e-5,
        0.70,
        4.0 * pi / 3.0,
        PlanetSurfaceType::Lava,
        303,
        250.0f,
        4.2f);
    lavaWorld.surfaceAnimationSpeed = 0.80f;
    planets.push_back(std::move(lavaWorld));

    RigidBody icyMoon = makeProceduralPlanet(
        "Procedural Icy Moon",
        3.7e-8,
        1.16e-5,
        6.0,
        4.75,
        PlanetSurfaceType::IcyMoon,
        404,
        1000.0f,
        5.2f);
    icyMoon.surfaceAnimationSpeed = 0.08f;
    icyMoon.surfaceColorA = glm::vec3(0.015f, 0.025f, 0.18f);
    icyMoon.surfaceColorB = glm::vec3(0.05f, 0.44f, 0.78f);
    icyMoon.surfaceColorC = glm::vec3(0.58f, 0.90f, 1.0f);
    icyMoon.surfaceColorD = glm::vec3(0.58f, 0.12f, 1.0f);
    icyMoon.surfaceColorE = glm::vec3(0.08f, 0.03f, 0.28f);
    icyMoon.surfaceColorF = glm::vec3(0.0f, 0.95f, 0.78f);
    icyMoon.surfaceColorG = glm::vec3(0.94f, 0.99f, 1.0f);
    planets.push_back(std::move(icyMoon));

    double totalMass = 1.0;
    glm::dvec3 weightedPosition(0.0);
    glm::dvec3 weightedVelocity(0.0);
    for (const RigidBody& planet : planets)
    {
        totalMass += planet.bodyMass;
        weightedPosition += planet.bodyMass * planet.position;
        weightedVelocity += planet.bodyMass * planet.velocity;
    }

    RigidBody star(1.0, 0.00465047);
    star.name = "Procedural Blue-White Star";
    star.position = -weightedPosition / totalMass;
    star.velocity = -weightedVelocity / totalMass;
    star.isStar = true;
    star.displayScale = 5.0f;
    star.starSurfacePreset = StarSurfacePreset::BlueWhite;
    star.starSeed = 2454345;
    star.starCellScale = 7.0f;
    star.starActivity = 1.15f;
    star.starAnimationSpeed = 0.65f;
    star.starSpotAmount = 0.30f;
    star.starCoronaSize = 0.13f;
    star.starCoronaIntensity = 1.15f;
    scene.sceneBodies.push_back(std::move(star));

    for (RigidBody& planet : planets)
    {
        planet.position += scene.sceneBodies.front().position;
        planet.velocity += scene.sceneBodies.front().velocity;
        scene.sceneBodies.push_back(std::move(planet));
    }

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
