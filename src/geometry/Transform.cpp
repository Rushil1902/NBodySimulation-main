#include "../graphics/shader.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "Transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Transform::Transform(Shader &shader): ourShader(shader), model(glm::mat4(1.0f)), scale(glm::mat4(1.0f)),rotate(glm::mat4(1.0f)), translate(glm::mat4(1.0f))
{
    
}

void Transform::setScale(const glm::vec3 &scaleVec){

    scale = glm::scale(scale, scaleVec);

}

void Transform::setTranslate(const glm::vec3 &translateVec){

    translate = glm::translate(translate, translateVec);

}

void Transform::setRotate(float angle, const glm::vec3 &rotateVec){

    rotate = glm::rotate(rotate,glm::radians(angle), rotateVec);
    

}

void Transform::applyTransformation() {

    ourShader.use();
    model = translate * rotate * scale;
    ourShader.setMat4("model", model);

    scale = glm::mat4(1.0f);   // Reset scale matrix to identity
    translate = glm::mat4(1.0f); // Reset translate matrix to identity
    rotate = glm::mat4(1.0f);    // Reset rotate matrix to identity
    model = glm::mat4(1.0f);    //rest model matrix to identity
}
