#pragma once
#include <iostream>
#include <vector>
#include <glad/glad.h>



class SkyboxMesh{

    public:
        SkyboxMesh();
        ~SkyboxMesh();
        void draw() const;
        void loadCubemap(std::vector<std::string>faces);

    private:
        unsigned int VAO, VBO;
        unsigned int textureID;

};