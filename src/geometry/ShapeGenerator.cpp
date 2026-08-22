#define _USE_MATH_DEFINES
#include "ShapeGenerator.hpp"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

MeshData ShapeGenerator::makeUnitCircle(int segments){

    MeshData data;

    data.vertices.insert(data.vertices.end(),{0.0f, 0.0f, 0.0f});

    for (int i = 0; i<=segments; i++){
        float angle  = 2.0f* (float)M_PI *i/segments;
        data.vertices.push_back(cos(angle));
        data.vertices.push_back(sin(angle));
        data.vertices.push_back(0.0f);

    }

    for (int i = 0; i<= segments; i++){
        data.indices.push_back(0);
        data.indices.push_back(i);
        data.indices.push_back(i+1);

    }

    return data;

}

MeshData ShapeGenerator::makeUnitSphere(int stacks, int sectors)
{

    MeshData data;

    for (int i =0; i <= stacks; i++)
    {
        float phi = M_PI * float(i)/float(stacks); // 0 to pi
        
        for (int j = 0; j<= sectors; j++)
        {
            float theta = 2 *M_PI * float(j)/float(sectors);

            //convert from spherical to cartesian coordinates

            float x = sin(phi) * cos(theta);
            float y = sin(phi) * sin(theta);
            float z = cos(phi);
            
            data.vertices.push_back(x);
            data.vertices.push_back(y);
            data.vertices.push_back(z);

            glm::vec3 normal = normalize(glm::vec3(x,y,z));

            data.vertices.push_back(normal.x);
            data.vertices.push_back(normal.y);
            data.vertices.push_back(normal.z);

            float u = (theta + M_PI)/ (2 *M_PI);
            float v = phi/ M_PI;

            data.vertices.push_back(u);
            data.vertices.push_back(v);
        }

    }

    for(int i = 0; i < stacks; i++)
    {
        for(int j = 0; j<sectors; j++)
        {
            //indices for the two triangles for each sector
            int first = i *(sectors +1) + j;
            int second = first + sectors + 1;

            //first triangle
            data.indices.push_back(first);
            data.indices.push_back(second);
            data.indices.push_back(first +1);

            //second triangle
            data.indices.push_back(second);
            data.indices.push_back(second +1);
            data.indices.push_back(first +1);
        }


    }


    return data;

}

MeshData ShapeGenerator::makeUnitSquare()
{
    MeshData data;

    // Position, normal, and UV data matching Mesh's interleaved layout.
    data.vertices = {
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
         0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f
    };
    data.indices = {0, 1, 2, 0, 2, 3};

    return data;
}

MeshData ShapeGenerator::makeUnitCube()
{
    MeshData data;

    // Vertices for a unit cube centered at the origin
    // The cube has 8 corners with coordinates ranging from -0.5 to 0.5 on all axes
    data.vertices.push_back(-0.5f);  // Front-bottom-left
    data.vertices.push_back(-0.5f);
    data.vertices.push_back(-0.5f);

    data.vertices.push_back(0.5f);   // Front-bottom-right
    data.vertices.push_back(-0.5f);
    data.vertices.push_back(-0.5f);

    data.vertices.push_back(0.5f);   // Back-bottom-right
    data.vertices.push_back(0.5f);
    data.vertices.push_back(-0.5f);

    data.vertices.push_back(-0.5f);  // Back-bottom-left
    data.vertices.push_back(0.5f);
    data.vertices.push_back(-0.5f);

    data.vertices.push_back(-0.5f);  // Front-top-left
    data.vertices.push_back(-0.5f);
    data.vertices.push_back(0.5f);

    data.vertices.push_back(0.5f);   // Front-top-right
    data.vertices.push_back(-0.5f);
    data.vertices.push_back(0.5f);

    data.vertices.push_back(0.5f);   // Back-top-right
    data.vertices.push_back(0.5f);
    data.vertices.push_back(0.5f);

    data.vertices.push_back(-0.5f);  // Back-top-left
    data.vertices.push_back(0.5f);
    data.vertices.push_back(0.5f);

    // Indices for the 12 triangles that make up the cube (2 triangles per face)
    // Front face
    data.indices.push_back(0); data.indices.push_back(1); data.indices.push_back(2);
    data.indices.push_back(0); data.indices.push_back(2); data.indices.push_back(3);

    // Back face
    data.indices.push_back(4); data.indices.push_back(5); data.indices.push_back(6);
    data.indices.push_back(4); data.indices.push_back(6); data.indices.push_back(7);

    // Left face
    data.indices.push_back(0); data.indices.push_back(3); data.indices.push_back(4);
    data.indices.push_back(3); data.indices.push_back(4); data.indices.push_back(7);

    // Right face
    data.indices.push_back(1); data.indices.push_back(2); data.indices.push_back(5);
    data.indices.push_back(2); data.indices.push_back(5); data.indices.push_back(6);

    // Top face
    data.indices.push_back(4); data.indices.push_back(5); data.indices.push_back(1);
    data.indices.push_back(4); data.indices.push_back(1); data.indices.push_back(0);

    // Bottom face
    data.indices.push_back(3); data.indices.push_back(2); data.indices.push_back(6);
    data.indices.push_back(3); data.indices.push_back(6); data.indices.push_back(7);

    return data;
};
