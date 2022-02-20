#pragma once
#ifndef Camera_H
#define Camera_H
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Transform.h"
#include <cmath>
#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\type_ptr.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>

class Camera :
    public Transform
{

public:

    //Methods
    Camera(float fieldOfView, int width, int height);
    void WindowReshapeCallback(GLFWwindow* window, int newWidth, int newHeight);

    virtual void UpdateMatrices(bool aroundOrigin);

    //Properties
    float fov;
    float fovScale;
    float apectRatio;

    glm::mat4 vMat;

    static Camera* activeCam;
};

#endif