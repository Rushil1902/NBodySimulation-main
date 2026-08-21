#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

class RigidBody{

    public:

        RigidBody(double massSolar, double radiusAU)
            : bodyMass(massSolar), bodyRadius(radiusAU), position(0.0),
              velocity(0.0), acceleration(0.0) {}

        double bodyMass;   // Solar masses
        double bodyRadius; // Astronomical units
        std::string name = "Body";
        std::string texturePath;
        
        glm::dvec3 position;     // AU
        glm::dvec3 velocity;     // AU / year
        glm::dvec3 acceleration; // AU / year^2
        bool fixed = false; //does the object remain fixed in place, false by default
        std::vector<glm::vec3> path;
        std::vector<float> pathAlphas;
        unsigned int textureID = 0;
        bool isStar = false;

        void setPosition(const glm::dvec3& vecPosition);
        void setVelocity(const glm::dvec3& vecVelocity);
        void setAcceleration(const glm::dvec3& vecAcceleration);
        void updatePath(const glm::vec3& renderPosition);
        void clearPath();
        void loadTexture(const std::string& filepath);
        void bindTexture() const;
    
};
