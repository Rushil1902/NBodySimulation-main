#pragma once

#include "physics/Physics.hpp"
#include "scene/Scene.hpp"

class Simulation {
public:
    explicit Simulation(Scene& scene);

    bool update(double realFrameSeconds);
    void togglePaused();
    void reset();
    void requestSingleStep();
    void increaseSpeed();
    void decreaseSpeed();

    bool isPaused() const;
    double elapsedYears() const;
    double yearsPerRealSecond() const;
    double relativeEnergyDrift() const;

private:
    void advanceOneStep();
    void validateState();

    static constexpr double fixedTimeStepYears = 0.001;
    static constexpr int maxPhysicsStepsPerFrame = 100;

    Scene& scene;
    Physics physicsEngine;
    double simulationRate = 0.06;
    double simulationTime = 0.0;
    double accumulatedTime = 0.0;
    double initialEnergy = 0.0;
    bool paused = false;
    bool stepRequested = false;
    bool numericalFailureReported = false;
};
