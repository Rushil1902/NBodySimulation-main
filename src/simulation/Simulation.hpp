#pragma once

#include "physics/Physics.hpp"
#include "scene/Scene.hpp"

class Simulation {
public:
    explicit Simulation(Scene& scene);

    bool update(double realFrameSeconds);
    void togglePaused();
    void setPaused(bool shouldPause);
    void reset();
    void requestSingleStep();
    void increaseSpeed();
    void decreaseSpeed();
    void setYearsPerRealSecond(double yearsPerSecond);
    void setTimeStepYears(double timeStepYears);
    void setSofteningLengthAU(double softeningLengthAU);
    void notifySceneChanged();

    bool isPaused() const;
    double elapsedYears() const;
    double yearsPerRealSecond() const;
    double timeStepYears() const;
    double softeningLengthAU() const;
    double totalEnergy() const;
    double relativeEnergyDrift() const;
    glm::dvec3 totalMomentum() const;
    glm::dvec3 totalAngularMomentum() const;

private:
    void advanceOneStep();
    void validateState();

    static constexpr int maxPhysicsStepsPerFrame = 100;

    Scene& scene;
    Physics physicsEngine;
    double physicsTimeStep = 0.001;
    double simulationRate = 0.06;
    double simulationTime = 0.0;
    double accumulatedTime = 0.0;
    double initialEnergy = 0.0;
    bool paused = false;
    bool stepRequested = false;
    bool numericalFailureReported = false;
};
