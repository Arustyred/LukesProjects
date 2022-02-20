#pragma once
#ifndef Input_H
#define Input_H
#include "GLFW/glfw3.h"
#include "Cursor.h"

class Input
{
public:
	static GLFWwindow* win;
	static void Update(float deltaTime);
	static void PostUpdate();
	static void UpdateTargets();

	static void KeyAction(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseButton(GLFWwindow* window, int button, int action, int mods);
	static void MouseMove(GLFWwindow* window, double xpos, double ypos);
	static void WindowReshape(GLFWwindow* window, int newWidth, int newHeight);

	static float interpolationSpeed;
	static float forward, sideways, rawForward, rawSideways;
	static float mouseX, mouseY, rawMouseX, rawMouseY;
	static bool w, a, s, d;
	static bool mouseLeftClick, mouseLeftHold, mouseRightClick, mouseRightHold;
};

#endif