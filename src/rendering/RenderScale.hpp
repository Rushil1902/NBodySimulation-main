#pragma once

#include <glm/glm.hpp>

namespace RenderScale {
constexpr float unitsPerAU = 4.0f;
constexpr float bodyRadiusExaggeration = 10.0f;

inline glm::vec3 position(const glm::dvec3& positionAU)
{
    return glm::vec3(positionAU * static_cast<double>(unitsPerAU));
}

inline float radius(double radiusAU)
{
    return static_cast<float>(radiusAU) * unitsPerAU * bodyRadiusExaggeration;
}
}
