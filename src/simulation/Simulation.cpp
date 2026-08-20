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
        while (accumulatedTime >= fixedTimeStepYears
               && physicsSteps < maxPhysicsStepsPerFrame)
        {
            advanceOneStep();
            accumulatedTime -= fixedTimeStepYears;
            ++physicsSteps;
        }

        physicsAdvanced = physicsSteps > 0;
        if (physicsSteps == maxPhysicsStepsPerFrame)
            accumulatedTime = std::fmod(accumulatedTime, fixedTimeStepYears);
    }

    validateState();
    return physicsAdvanced;
}

void Simulation::togglePaused()
{
    paused = !paused;
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

double Simulation::relativeEnergyDrift() const
{
    if (initialEnergy == 0.0)
        return 0.0;

    return (physicsEngine.totalEnergy() - initialEnergy) / std::abs(initialEnergy);
}

void Simulation::advanceOneStep()
{
    physicsEngine.updatePhysics(fixedTimeStepYears);
    simulationTime += fixedTimeStepYears;
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
