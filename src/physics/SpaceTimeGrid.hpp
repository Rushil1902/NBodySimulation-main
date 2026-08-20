#pragma once
#include <iostream>
#include "RigidBody.hpp"
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class SpaceTimeGrid {

    public:
        SpaceTimeGrid(float gridSize, int baseResolution);
        void updateGrid(const std::vector<RigidBody>& bodies);
        void generatePolarGridLines(int radialDivs, int angularDivs);
        void draw() const;


    private:
        float gridSize;
        int baseResolution;
        unsigned int VAO, VBO, EBO;
        std::vector<float> gridVertices;
        std::vector<unsigned int> gridIndices;
        static constexpr double G = 6.67430e-11;
        static constexpr double c = 299792458.0;
        static constexpr double solarMassKg = 1.98847e30;
        static constexpr double astronomicalUnitMeters = 1.495978707e11;
        static constexpr double renderUnitsPerAU = 4.0;
};
