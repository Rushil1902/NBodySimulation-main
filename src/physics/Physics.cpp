#include "Physics.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

Physics::Physics(std::vector<RigidBody>& bodies, double softeningLengthAU)
    : rigidBodies(bodies), softeningLength(std::max(1.0e-12, softeningLengthAU))
{
}

glm::dvec3 Physics::calculateAcceleration(std::size_t bodyIndex) const
{
    glm::dvec3 netAcceleration(0.0);
    const RigidBody& body = rigidBodies[bodyIndex];
    const double softeningSquared = softeningLength * softeningLength;

    for (std::size_t otherIndex = 0; otherIndex < rigidBodies.size(); ++otherIndex)
    {
        if (bodyIndex == otherIndex)
            continue;

        const RigidBody& other = rigidBodies[otherIndex];
        const glm::dvec3 displacement = other.position - body.position;
        const double softenedDistanceSquared = glm::dot(displacement, displacement) + softeningSquared;
        const double inverseDistance = 1.0 / std::sqrt(softenedDistanceSquared);
        const double inverseDistanceCubed = inverseDistance * inverseDistance * inverseDistance;

        netAcceleration += gravitationalConstant * other.bodyMass * displacement * inverseDistanceCubed;
    }

    return netAcceleration;
}

void Physics::updatePhysics(double timeStepYears)
{
    if (timeStepYears <= 0.0 || rigidBodies.empty())
        return;

    std::vector<glm::dvec3> currentAccelerations(rigidBodies.size());
    std::vector<glm::dvec3> newAccelerations(rigidBodies.size());

    for (std::size_t i = 0; i < rigidBodies.size(); ++i)
        currentAccelerations[i] = calculateAcceleration(i);

    for (std::size_t i = 0; i < rigidBodies.size(); ++i)
    {
        RigidBody& body = rigidBodies[i];
        if (body.fixed)
            continue;

        body.position += body.velocity * timeStepYears
            + 0.5 * currentAccelerations[i] * timeStepYears * timeStepYears;
    }

    for (std::size_t i = 0; i < rigidBodies.size(); ++i)
        newAccelerations[i] = calculateAcceleration(i);

    for (std::size_t i = 0; i < rigidBodies.size(); ++i)
    {
        RigidBody& body = rigidBodies[i];
        if (!body.fixed)
            body.velocity += 0.5 * (currentAccelerations[i] + newAccelerations[i]) * timeStepYears;

        body.acceleration = newAccelerations[i];
    }
}

double Physics::totalEnergy() const
{
    double energy = 0.0;
    const double softeningSquared = softeningLength * softeningLength;

    for (const RigidBody& body : rigidBodies)
        energy += 0.5 * body.bodyMass * glm::dot(body.velocity, body.velocity);

    for (std::size_t i = 0; i < rigidBodies.size(); ++i)
    {
        for (std::size_t j = i + 1; j < rigidBodies.size(); ++j)
        {
            const glm::dvec3 displacement = rigidBodies[j].position - rigidBodies[i].position;
            const double softenedDistance = std::sqrt(glm::dot(displacement, displacement) + softeningSquared);
            energy -= gravitationalConstant * rigidBodies[i].bodyMass * rigidBodies[j].bodyMass / softenedDistance;
        }
    }

    return energy;
}

glm::dvec3 Physics::totalMomentum() const
{
    glm::dvec3 momentum(0.0);
    for (const RigidBody& body : rigidBodies)
        momentum += body.bodyMass * body.velocity;
    return momentum;
}

glm::dvec3 Physics::totalAngularMomentum() const
{
    glm::dvec3 angularMomentum(0.0);
    for (const RigidBody& body : rigidBodies)
        angularMomentum += body.bodyMass * glm::cross(body.position, body.velocity);
    return angularMomentum;
}

bool Physics::hasFiniteState() const
{
    for (const RigidBody& body : rigidBodies)
    {
        for (int component = 0; component < 3; ++component)
        {
            if (!std::isfinite(body.position[component])
                || !std::isfinite(body.velocity[component])
                || !std::isfinite(body.acceleration[component]))
                return false;
        }
    }
    return true;
}

double Physics::getSofteningLength() const
{
    return softeningLength;
}

void Physics::setSofteningLength(double softeningLengthAU)
{
    softeningLength = std::max(1.0e-12, softeningLengthAU);
}
