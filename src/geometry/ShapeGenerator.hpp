#pragma once
#include "MeshData.hpp"

class ShapeGenerator{

public:
    static MeshData makeUnitCircle(int segments);
    static MeshData makeUnitSphere(int stacks, int sectors);
    static MeshData makeUnitSquare();
    static MeshData makeUnitCube();
};