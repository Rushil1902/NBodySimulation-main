#include "TrailMesh.hpp"
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>



TrailMesh::TrailMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1,&VBO);
    

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Alpha attribute (location 1)
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void TrailMesh::updateTrail(std::vector<glm::vec3>& path, std::vector<float>& pathAlphas)
{
    //update path count
    pathCount = path.size();

    //clear old data
    pathVertices.clear(); 

    // Interleave: x, y, z, alpha, x, y, z, alpha...
    for(size_t i = 0; i < path.size(); i++)
    {
        pathVertices.push_back(path[i].x);
        pathVertices.push_back(path[i].y);
        pathVertices.push_back(path[i].z);
        pathVertices.push_back(pathAlphas[i]);  // Add alpha
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, pathVertices.size()*sizeof(float), pathVertices.data(), GL_DYNAMIC_DRAW); 
}

void TrailMesh::draw()const{

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP,0, pathCount);

}
