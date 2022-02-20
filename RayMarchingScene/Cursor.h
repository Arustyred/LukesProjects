#pragma once
#ifndef Cursor_H
#define Cursor_H
#include "GLFW/glfw3.h"

class Cursor {

public:

    //Methods
    static void Init(GLFWwindow* w);
    static void Update();
    static void PostUpdate();
    static void MouseMoveCallback(float xPos, float yPos);
    static void SetVisible(bool v);

    static bool visible;
    static float dx, dy;

protected:
    //Properties
    static float lx, ly;
    static bool reset, firstFrame;
    static GLFWwindow* window;
};

#endif