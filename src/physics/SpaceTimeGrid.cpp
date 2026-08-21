#include "SpaceTimeGrid.hpp"

#include "RigidBody.hpp"
#include "rendering/RenderScale.hpp"

#include <algorithm>
#include <cmath>

SpaceTimeGrid::SpaceTimeGrid(int resolution)
    : resolution(std::max(resolution, 2))
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &lineEBO);
    glGenBuffers(1, &triangleEBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1,
        1,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    generateIndices();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineEBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(lineIndices.size() * sizeof(unsigned int)),
        lineIndices.data(),
        GL_STATIC_DRAW);
    glBindVertexArray(0);
}

SpaceTimeGrid::~SpaceTimeGrid()
{
    glDeleteBuffers(1, &triangleEBO);
    glDeleteBuffers(1, &lineEBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void SpaceTimeGrid::update(
    const std::vector<RigidBody>& bodies,
    const GravityFieldSettings& settings)
{
    const int verticesPerSide = resolution + 1;
    const float widthAU = std::max(settings.widthAU, 0.1f);
    const double softeningAU = std::max(static_cast<double>(settings.softeningAU), 1.0e-6);
    const double softeningSquared = softeningAU * softeningAU;
    const double fieldStrength = std::max(static_cast<double>(settings.fieldStrength), 0.0);
    const float halfWidthAU = widthAU * 0.5f;

    vertices.clear();
    vertices.reserve(static_cast<std::size_t>(verticesPerSide * verticesPerSide * 4));

    for (int row = 0; row <= resolution; ++row)
    {
        const double yAU = -halfWidthAU
            + widthAU * static_cast<double>(row) / static_cast<double>(resolution);

        for (int column = 0; column <= resolution; ++column)
        {
            const double xAU = -halfWidthAU
                + widthAU * static_cast<double>(column) / static_cast<double>(resolution);

            double potential = 0.0;
            for (const RigidBody& body : bodies)
            {
                const double dx = xAU - body.position.x;
                const double dy = yAU - body.position.y;
                const double dz = settings.sliceZAU - body.position.z;
                const double distance = std::sqrt(dx * dx + dy * dy + dz * dz + softeningSquared);
                potential += std::max(body.bodyMass, 0.0) / distance;
            }

            // The saturating map keeps the illustrative well finite even when a
            // body lies exactly on a grid vertex.
            const float normalizedField = static_cast<float>(
                1.0 - std::exp(-fieldStrength * potential));
            const float depth = settings.flatHeatmap
                ? 0.0f
                : std::max(settings.maximumDepth, 0.0f) * normalizedField;

            vertices.push_back(static_cast<float>(xAU) * RenderScale::unitsPerAU);
            vertices.push_back(static_cast<float>(yAU) * RenderScale::unitsPerAU);
            vertices.push_back(
                static_cast<float>(settings.sliceZAU) * RenderScale::unitsPerAU + depth);
            vertices.push_back(normalizedField);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
        vertices.data(),
        GL_DYNAMIC_DRAW);
}

void SpaceTimeGrid::drawSurface() const
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleEBO);
    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(triangleIndices.size()),
        GL_UNSIGNED_INT,
        nullptr);
    glBindVertexArray(0);
}

void SpaceTimeGrid::drawLines() const
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineEBO);
    glDrawElements(
        GL_LINES,
        static_cast<GLsizei>(lineIndices.size()),
        GL_UNSIGNED_INT,
        nullptr);
    glBindVertexArray(0);
}

void SpaceTimeGrid::generateIndices()
{
    const int verticesPerSide = resolution + 1;

    for (int row = 0; row <= resolution; ++row)
    {
        for (int column = 0; column < resolution; ++column)
        {
            const unsigned int index = static_cast<unsigned int>(
                row * verticesPerSide + column);
            lineIndices.push_back(index);
            lineIndices.push_back(index + 1);
        }
    }

    for (int column = 0; column <= resolution; ++column)
    {
        for (int row = 0; row < resolution; ++row)
        {
            const unsigned int index = static_cast<unsigned int>(
                row * verticesPerSide + column);
            lineIndices.push_back(index);
            lineIndices.push_back(index + static_cast<unsigned int>(verticesPerSide));
        }
    }

    for (int row = 0; row < resolution; ++row)
    {
        for (int column = 0; column < resolution; ++column)
        {
            const unsigned int topLeft = static_cast<unsigned int>(
                row * verticesPerSide + column);
            const unsigned int topRight = topLeft + 1;
            const unsigned int bottomLeft = topLeft
                + static_cast<unsigned int>(verticesPerSide);
            const unsigned int bottomRight = bottomLeft + 1;

            triangleIndices.insert(
                triangleIndices.end(),
                {topLeft, bottomLeft, topRight, topRight, bottomLeft, bottomRight});
        }
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleEBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(triangleIndices.size() * sizeof(unsigned int)),
        triangleIndices.data(),
        GL_STATIC_DRAW);
}
