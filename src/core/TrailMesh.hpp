#pragma once
#include <vector>
#include <glm/glm.hpp>

class TrailMesh{

    public:

        TrailMesh();
        void updateTrail(std::vector<glm::vec3>& path, std::vector<float>& pathAlphas);
        void draw() const;


    private:

        unsigned int VBO, VAO;
        unsigned int pathCount;
        std::vector<float> pathVertices;


};