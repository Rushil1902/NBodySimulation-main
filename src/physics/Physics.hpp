#pragma once
#include "RigidBody.hpp"
#include <glm/glm.hpp>
#include <cstddef>
#include <vector>

class Physics{

    public:

        static constexpr double gravitationalConstant = 39.47841760435743; // AU^3 / (solar mass * year^2)

        explicit Physics(std::vector<RigidBody>& bodies, double softeningLengthAU = 1.0e-5);
        void updatePhysics(double timeStepYears);

        double totalEnergy() const;
        glm::dvec3 totalMomentum() const;
        glm::dvec3 totalAngularMomentum() const;
        bool hasFiniteState() const;

        double getSofteningLength() const;
        void setSofteningLength(double softeningLengthAU);

        std::vector<RigidBody>& rigidBodies;

    private:
        glm::dvec3 calculateAcceleration(std::size_t bodyIndex) const;
        double softeningLength;
};
