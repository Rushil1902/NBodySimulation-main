#pragma once
#include <glad/glad.h>
#include "../geometry/MeshData.hpp"

class Mesh {
public:
    Mesh(const MeshData& data);
    ~Mesh();

    void draw() const;
    void drawPath();

    private:
        unsigned int VBO, VAO, EBO;
        unsigned int indexCount;

};