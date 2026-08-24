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

enum class StarSurfacePreset {
        Solar = 0,
        BlueWhite,
        RedDwarf,
        Alien
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
        StarSurfacePreset starSurfacePreset = StarSurfacePreset::BlueWhite;
        int starSeed = 2454345;
        float starCellScale = 7.0f;
        float starActivity = 1.0f;
        float starAnimationSpeed = 0.65f;
        float starSpotAmount = 0.28f;
        float starCoronaSize = 0.12f;
        float starCoronaIntensity = 1.0f;
        glm::vec3 starColorA = glm::vec3(0.005f, 0.05f, 0.14f);
        glm::vec3 starColorB = glm::vec3(0.0f, 0.30f, 0.48f);
        glm::vec3 starColorC = glm::vec3(0.0f, 0.68f, 0.72f);
        glm::vec3 starColorD = glm::vec3(0.38f, 0.90f, 0.82f);
        glm::vec3 starColorE = glm::vec3(1.0f, 0.98f, 0.70f);
        glm::vec3 starColorF = glm::vec3(1.0f, 1.0f, 0.95f);
        glm::vec3 starColorG = glm::vec3(0.12f, 0.85f, 1.0f);
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
