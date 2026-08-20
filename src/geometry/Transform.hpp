#pragma once
#include "glm/fwd.hpp"
#include "../graphics/shader.h"


class Transform{

    public:
        
        Transform(Shader &Shader);
        void setScale(const glm::vec3 &scaleVec);
        void setTranslate(const glm::vec3 &translateVec);
        void setRotate(float angle, const glm::vec3 &rotateVec);
        void applyTransformation();

    private:
    Shader &ourShader;          // Reference to the shader program
    glm::mat4 scale;
    glm::mat4 model;           // Model transformation matrix
    glm::mat4 rotate;
    glm::mat4 translate;

};