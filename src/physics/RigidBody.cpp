#include "RigidBody.hpp"
#include "core/AssetPaths.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <filesystem>
#include <iostream>
#include <stb_image.h>

void RigidBody::setPosition(const glm::dvec3& vecPosition)
{
    position = vecPosition;
}

void RigidBody::setVelocity(const glm::dvec3& vecVelocity)
{
    velocity = vecVelocity;
}

void RigidBody::setAcceleration(const glm::dvec3& vecAcceleration)
{
    acceleration = vecAcceleration;
}

void RigidBody::updatePath(const glm::vec3& renderPosition)
{
    path.push_back(renderPosition);
    pathAlphas.push_back(1.0f);

    //fade path
    for(float& alpha : pathAlphas) 
    {
            alpha -= 0.005f;
    }

    while (!path.empty() && pathAlphas.front() <= 0.0f)
    {
        path.erase(path.begin());
        pathAlphas.erase(pathAlphas.begin());
    }
}

void RigidBody::clearPath()
{
    path.clear();
    pathAlphas.clear();
}

void RigidBody::loadTexture(const std::string& filepath)
{
    texturePath = filepath;
    std::filesystem::path resolvedPath;
    try
    {
        resolvedPath = AssetPaths::resolve(filepath);
    }
    catch (const std::exception& error)
    {
        std::cerr << "Failed to load texture: " << error.what() << std::endl;
        return;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    //Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(resolvedPath.string().c_str(), &width, &height, &nrChannels, 0);
    if (data){
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0 , format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load texture: " << resolvedPath << std::endl;
    }
}

void RigidBody::bindTexture() const{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
}
