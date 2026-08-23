#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

enum class PlanetSurfaceType {
        Texture = 0,
        EarthLike,
        GasGiant,
        Lava,
        IcyMoon
};

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
        float displayScale = 1.0f;
        PlanetSurfaceType surfaceType = PlanetSurfaceType::Texture;
        int proceduralSeed = 1;
        float terrainScale = 3.0f;
        float oceanLevel = 0.5f;
        float cloudCoverage = 0.5f;
        float surfaceAnimationSpeed = 1.0f;
        glm::vec3 surfaceColorA = glm::vec3(0.02f, 0.30f, 0.48f);
        glm::vec3 surfaceColorB = glm::vec3(0.08f, 0.30f, 0.10f);
        glm::vec3 surfaceColorC = glm::vec3(0.78f, 0.75f, 0.67f);
        glm::vec3 surfaceColorD = glm::vec3(0.48f, 0.82f, 0.08f);
        glm::vec3 surfaceColorE = glm::vec3(0.64f, 0.38f, 0.82f);
        glm::vec3 surfaceColorF = glm::vec3(0.88f, 0.94f, 1.0f);
        glm::vec3 surfaceColorG = glm::vec3(0.98f, 0.99f, 1.0f);
        float surfaceVibrancy = 1.35f;
        float surfaceColorBanding = 0.55f;

        void setPosition(const glm::dvec3& vecPosition);
        void setVelocity(const glm::dvec3& vecVelocity);
        void setAcceleration(const glm::dvec3& vecAcceleration);
        void updatePath(const glm::vec3& renderPosition);
        void clearPath();
        void loadTexture(const std::string& filepath);
        void bindTexture() const;
    
};
