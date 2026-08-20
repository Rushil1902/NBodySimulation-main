#pragma once
#include "shader.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>



class Camera{

    public:

        Camera (Shader& shader);
        static void scroll_callback_static(GLFWwindow* window, double xOffset, double yOffset);
        static void mouse_button_callback_static(GLFWwindow* window, int button, int action, int mods);
        static void cursor_position_callback_static(GLFWwindow* window, double xpos, double ypos);
        void scroll_callback(double xOffset, double yOffset);
        void mouse_button_callback(GLFWwindow* window,int button, int action, int mods);
        void cursor_position_callback(double xpos, double ypos);
        void setCameraPosition(glm::vec3 camPosition);
        void setPerspective(float fov, float aspectRation, float nearFrame, float farFrame);
        void updateCameraPosition();
        glm::mat4 getViewMatrix();
        glm::mat4 getPerspectiveMatrix();
        glm::vec3 position;

        
    private:

        
        float rotationX, rotationY; //in radians
        bool isShiftDragging = false, isDragging = false;
        double lastMouseX, lastMouseY;
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
        Shader& ourShader;
};
