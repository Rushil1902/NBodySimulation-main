#include "camera.hpp"
#include "shader.h"
#include <glm/glm.hpp>
#include "glm/fwd.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>


Camera::Camera(Shader& shader):ourShader(shader),position(1.0f),viewMatrix(1.0f),projectionMatrix(1.0f)
,rotationX(0.0f),rotationY(0.0f)
{

}

void Camera::updateCameraPosition()
{
    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::rotate(viewMatrix,rotationX,glm::vec3(0.0f,1.0f,0.0f));
    viewMatrix = glm::rotate(viewMatrix,rotationY,glm::vec3(1.0f,0.0f,0.0f));
    viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f,0.0f,position.z));
    
    //calculate orbit axis

    viewMatrix = glm::rotate(viewMatrix,position.x,glm::vec3(0.0f,1.0f,0.0f));
    viewMatrix = glm::rotate(viewMatrix,position.y,glm::vec3(1.0f,0.0f,0.0f));
    ourShader.setMat4("view",viewMatrix);    
}
void Camera::setCameraPosition(glm::vec3 camPosition)
{
    viewMatrix  = glm::translate(viewMatrix, camPosition);
    ourShader.setMat4("view", viewMatrix);
    position = camPosition;

}

void Camera::setPerspective(float fov, float aspectRatio, float nearFrame, float farFrame)
{
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(fov, aspectRatio, nearFrame, farFrame);
    ourShader.setMat4("projection", projection);
    
    projectionMatrix = projection;
}

void Camera::scroll_callback_static(GLFWwindow* window, double xOffset, double yOffset)
{
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if (camera) {
        camera->scroll_callback(xOffset, yOffset);  // Call the real function
    }   
}

void Camera::scroll_callback(double xOffset, double yOffset)
{
    position.z += yOffset * 1.0f;

    if (position.z > -1.0f) position.z = -1.0f;
    if (position.z < -200.0f) position.z = -200.0f;
    

}

void Camera::mouse_button_callback_static(GLFWwindow* window, int button, int action, int mods)
{
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if (camera) {
        camera->mouse_button_callback(window,button, action, mods);  // Call the real function
    }   
}

void Camera::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            // Check if shift is held down
            if (mods & GLFW_MOD_SHIFT)
            {
                isShiftDragging = true;
                std::cout << "Shift-drag started (pan mode)" << std::endl;
            }
            else
            {
                isDragging = true;
                std::cout << "Drag started (rotate mode)" << std::endl;
            }
            
            // Save starting mouse position
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        }
        else if (action == GLFW_RELEASE)
        {
            isDragging = false;
            isShiftDragging = false;
            std::cout << "Drag ended" << std::endl;
        }
    }
}

void Camera::cursor_position_callback_static(GLFWwindow* window, double xpos, double ypos)
{
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if (camera) {
        camera->cursor_position_callback(xpos, ypos);  // Call the real function
    }   
}

void Camera::cursor_position_callback(double xpos, double ypos)
{
    if (isDragging)
    {
        double deltaX = xpos - lastMouseX;
        double deltaY = ypos - lastMouseY;
        
        rotationX -= deltaX * 1/192.0f;
        rotationY -= deltaY * 1/192.0f;

        lastMouseX = xpos;
        lastMouseY = ypos; 
    }
    if (isShiftDragging)
    {
        double deltaX = xpos - lastMouseX;
        double deltaY = ypos - lastMouseY;

        position.x += deltaX*0.01;
        position.y += deltaY*0.01;

        lastMouseX = xpos;
        lastMouseY = ypos;
    }
}

glm::mat4 Camera::getPerspectiveMatrix()
{
    return projectionMatrix;
}

glm::mat4 Camera::getViewMatrix()
{
    return viewMatrix;
}