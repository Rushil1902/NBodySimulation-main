#define _USE_MATH_DEFINES
#include "SpaceTimeGrid.hpp"
#include <cmath>
#include "RigidBody.hpp"
#include <glm/glm.hpp>




SpaceTimeGrid::SpaceTimeGrid(float gridSize, int baseResolution): gridSize(gridSize), baseResolution(baseResolution)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 
    
    glBindVertexArray(0);
}

void SpaceTimeGrid::updateGrid(const std::vector<RigidBody>& bodies)
{
    gridVertices.clear();
    gridIndices.clear();

    int radialDivisions = baseResolution;     // Number of rings
    int angularDivisions = baseResolution * 2; // Number of spokes (more for smooth circle)
    
    float maxRadius = gridSize / 2.0f;
    
    // Generate polar grid vertices
    for(int i = 0; i <= radialDivisions; i++) {
        float r = (maxRadius * i) / radialDivisions;  // 0 to maxRadius
        
        for(int j = 0; j <= angularDivisions; j++) {
            float theta = (2.0f * M_PI * j) / angularDivisions;  // 0 to 2π
            
            // Convert polar to cartesian
            float x = r * cos(theta);
            float z = r * sin(theta);
            float y = 0.0f;

            for(const RigidBody& body : bodies) {
                const double bodyRenderX = body.position.x * renderUnitsPerAU;
                const double bodyRenderZ = body.position.z * renderUnitsPerAU;
                const double distRender = glm::length(glm::dvec2(x - bodyRenderX, z - bodyRenderZ));
                const double dist = (distRender / renderUnitsPerAU) * astronomicalUnitMeters;
                const double r_s = 2.0 * G * (body.bodyMass * solarMassKg) / (c * c);
                 
                if(dist > r_s && dist > 0.01) {
                    // Calculate Flamm displacement
                    const double z_flamm = 2.0 * sqrt(r_s * (dist - r_s));
                     
                    // Apply exponential decay based on distance from body
                    const double decayLength = (gridSize / renderUnitsPerAU) * astronomicalUnitMeters;
                    const double decayFactor = exp(-dist / decayLength);
                     
                    y += static_cast<float>(0.000001 * z_flamm * decayFactor - 40.0);
                }
            }            

            gridVertices.push_back(x);
            gridVertices.push_back(y);
            gridVertices.push_back(z);



        }
    }


    
    // Generate line indices for grid pattern
    generatePolarGridLines(radialDivisions, angularDivisions);

    

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gridIndices.size() * sizeof(unsigned int), gridIndices.data(), GL_DYNAMIC_DRAW); 

    
    
}

void SpaceTimeGrid::generatePolarGridLines(int radialDivs, int angularDivs)
{
    for(int j = 0; j < angularDivs; j++) {
        for(int i = 0; i < radialDivs; i++) {
            int idx = i * (angularDivs + 1) + j;
            gridIndices.push_back(idx);
            gridIndices.push_back(idx + (angularDivs + 1));
        }
    }
    
    // Circular lines (rings at each radius)
    for(int i = 0; i <= radialDivs; i++) {
        for(int j = 0; j < angularDivs; j++) {
            int idx = i * (angularDivs + 1) + j;
            gridIndices.push_back(idx);
            gridIndices.push_back(idx + 1);
        }
    }
}

void SpaceTimeGrid::draw() const {
    
    glBindVertexArray(VAO);
    glDrawElements(GL_LINES, gridIndices.size(), GL_UNSIGNED_INT, 0);
    
}
