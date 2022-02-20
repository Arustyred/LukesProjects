#include "Camera.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <cmath>
#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\type_ptr.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>

Camera::Camera(float fieldOfView, int width, int height) {
    fov = fieldOfView;
    fovScale = tan(glm::radians(fov / 2.0f));
    apectRatio = (float)width / (float)height;
    UpdateMatrices(true);

    if (activeCam == NULL)
        activeCam = this;
}

void Camera::UpdateMatrices(bool aroundOrigin)
{
    rMat = glm::rotate(glm::mat4(1.0f), glm::radians(eulerAngles.y), glm::vec3(0, 1, 0));
    rMat *= glm::rotate(glm::mat4(1.0f), glm::radians(eulerAngles.x), glm::vec3(1, 0, 0));
    rMat *= glm::rotate(glm::mat4(1.0f), glm::radians(eulerAngles.z), glm::vec3(0, 0, 1));
    tMat = glm::translate(glm::mat4(1.0f), position);

    vMat = tMat * rMat;
}

void Camera::WindowReshapeCallback(GLFWwindow* window, int newWidth, int newHeight) {
    apectRatio = (float)newWidth / (float)newHeight;
    glViewport(0, 0, newWidth, newHeight);
}