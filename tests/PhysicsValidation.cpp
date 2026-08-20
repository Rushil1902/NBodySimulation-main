#include "physics/Physics.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

int main()
{
    constexpr double solarMass = 1.0;
    constexpr double earthMass = 3.0034896e-6;
    constexpr double separationAU = 1.0;
    constexpr double timeStepYears = 0.0005;
    constexpr double durationYears = 10.0;

    const double totalMass = solarMass + earthMass;
    const double angularSpeed = std::sqrt(
        Physics::gravitationalConstant * totalMass
        / (separationAU * separationAU * separationAU));

    RigidBody sun(solarMass, 0.00465047);
    RigidBody earth(earthMass, 4.26352e-5);

    sun.position = glm::dvec3(-earthMass / totalMass * separationAU, 0.0, 0.0);
    earth.position = glm::dvec3(solarMass / totalMass * separationAU, 0.0, 0.0);
    sun.velocity = glm::dvec3(0.0, 0.0, angularSpeed * sun.position.x);
    earth.velocity = glm::dvec3(0.0, 0.0, angularSpeed * earth.position.x);

    std::vector<RigidBody> bodies{sun, earth};
    Physics physics(bodies, 1.0e-5);

    const double initialEnergy = physics.totalEnergy();
    const glm::dvec3 initialAngularMomentum = physics.totalAngularMomentum();
    const int stepCount = static_cast<int>(durationYears / timeStepYears);
    double maximumSeparationError = 0.0;

    for (int step = 0; step < stepCount; ++step)
    {
        physics.updatePhysics(timeStepYears);
        if (!physics.hasFiniteState())
        {
            std::cerr << "FAIL: non-finite simulation state at step " << step << '\n';
            return 1;
        }

        const double currentSeparation = glm::length(bodies[1].position - bodies[0].position);
        maximumSeparationError = std::max(
            maximumSeparationError,
            std::abs(currentSeparation - separationAU));
    }

    const double relativeEnergyDrift = std::abs(
        (physics.totalEnergy() - initialEnergy) / initialEnergy);
    const double relativeAngularMomentumDrift = glm::length(
        physics.totalAngularMomentum() - initialAngularMomentum)
        / glm::length(initialAngularMomentum);

    std::cout << "Two-body validation after " << durationYears << " years\n"
              << "  relative energy drift: " << relativeEnergyDrift << '\n'
              << "  relative angular momentum drift: " << relativeAngularMomentumDrift << '\n'
              << "  maximum separation error (AU): " << maximumSeparationError << '\n';

    if (relativeEnergyDrift > 5.0e-6
        || relativeAngularMomentumDrift > 1.0e-10
        || maximumSeparationError > 1.0e-3)
    {
        std::cerr << "FAIL: orbit stability exceeded its tolerance\n";
        return 1;
    }

    std::cout << "PASS\n";
    return 0;
}
