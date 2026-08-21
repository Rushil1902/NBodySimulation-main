#pragma once

#include <glad/glad.h>

#include <vector>

class RigidBody;

struct GravityFieldSettings {
    double sliceZAU = 0.0;
    float widthAU = 16.0f;
    float softeningAU = 0.25f;
    float fieldStrength = 0.35f;
    float maximumDepth = 8.0f;
    float surfaceOpacity = 0.18f;
    bool flatHeatmap = false;
    bool surfaceVisible = true;
};

class SpaceTimeGrid {
public:
    explicit SpaceTimeGrid(int resolution);
    ~SpaceTimeGrid();

    SpaceTimeGrid(const SpaceTimeGrid&) = delete;
    SpaceTimeGrid& operator=(const SpaceTimeGrid&) = delete;

    void update(const std::vector<RigidBody>& bodies, const GravityFieldSettings& settings);
    void drawSurface() const;
    void drawLines() const;

private:
    void generateIndices();

    int resolution;
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int lineEBO = 0;
    unsigned int triangleEBO = 0;
    std::vector<float> vertices;
    std::vector<unsigned int> lineIndices;
    std::vector<unsigned int> triangleIndices;
};
