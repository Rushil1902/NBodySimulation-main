#include "Simulation.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

Simulation::Simulation(Scene& scene)
    : scene(scene), physicsEngine(scene.bodies()), initialEnergy(physicsEngine.totalEnergy())
{
}

bool Simulation::update(double realFrameSeconds)
{
    bool physicsAdvanced = false;

    if (stepRequested)
    {
        advanceOneStep();
        physicsAdvanced = true;
        stepRequested = false;
    }
    else if (!paused)
    {
        const double boundedFrameTime = std::clamp(realFrameSeconds, 0.0, 0.25);
        accumulatedTime += boundedFrameTime * simulationRate;

        int physicsSteps = 0;
        while (accumulatedTime >= physicsTimeStep
               && physicsSteps < maxPhysicsStepsPerFrame)
        {
            advanceOneStep();
            accumulatedTime -= physicsTimeStep;
            ++physicsSteps;
        }

        physicsAdvanced = physicsSteps > 0;
        if (physicsSteps == maxPhysicsStepsPerFrame)
            accumulatedTime = std::fmod(accumulatedTime, physicsTimeStep);
    }

    validateState();
    return physicsAdvanced;
}

void Simulation::togglePaused()
{
    setPaused(!paused);
}

void Simulation::setPaused(bool shouldPause)
{
    paused = shouldPause;
    accumulatedTime = 0.0;
}

void Simulation::reset()
{
    scene.reset();
    simulationTime = 0.0;
    accumulatedTime = 0.0;
    initialEnergy = physicsEngine.totalEnergy();
    paused = true;
    stepRequested = false;
    numericalFailureReported = false;
}

void Simulation::requestSingleStep()
{
    if (paused)
        stepRequested = true;
}

void Simulation::increaseSpeed()
{
    simulationRate = std::min(simulationRate * 2.0, 64.0);
}

void Simulation::decreaseSpeed()
{
    simulationRate = std::max(simulationRate * 0.5, 0.001);
}

void Simulation::setYearsPerRealSecond(double yearsPerSecond)
{
    simulationRate = std::clamp(yearsPerSecond, 0.001, 64.0);
}

void Simulation::setTimeStepYears(double timeStepYears)
{
    physicsTimeStep = std::clamp(timeStepYears, 1.0e-6, 0.1);
    accumulatedTime = 0.0;
}

void Simulation::setSofteningLengthAU(double softeningLengthAU)
{
    physicsEngine.setSofteningLength(softeningLengthAU);
    notifySceneChanged();
}

void Simulation::notifySceneChanged()
{
    initialEnergy = physicsEngine.totalEnergy();
    accumulatedTime = 0.0;
    numericalFailureReported = false;
    paused = true;
}

bool Simulation::isPaused() const
{
    return paused;
}

double Simulation::elapsedYears() const
{
    return simulationTime;
}

double Simulation::yearsPerRealSecond() const
{
    return simulationRate;
}

double Simulation::timeStepYears() const
{
    return physicsTimeStep;
}

double Simulation::softeningLengthAU() const
{
    return physicsEngine.getSofteningLength();
}

double Simulation::totalEnergy() const
{
    return physicsEngine.totalEnergy();
}

double Simulation::relativeEnergyDrift() const
{
    if (initialEnergy == 0.0)
        return 0.0;

    return (physicsEngine.totalEnergy() - initialEnergy) / std::abs(initialEnergy);
}

glm::dvec3 Simulation::totalMomentum() const
{
    return physicsEngine.totalMomentum();
}

glm::dvec3 Simulation::totalAngularMomentum() const
{
    return physicsEngine.totalAngularMomentum();
}

void Simulation::advanceOneStep()
{
    physicsEngine.updatePhysics(physicsTimeStep);
    simulationTime += physicsTimeStep;
}

void Simulation::validateState()
{
    if (physicsEngine.hasFiniteState())
        return;

    paused = true;
    if (!numericalFailureReported)
    {
        std::cerr << "Simulation paused: a body reached a non-finite state.\n";
        numericalFailureReported = true;
    }
}
