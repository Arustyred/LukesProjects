#include "Cursor.h"
#include "GLFW/glfw3.h"

bool Cursor::visible = false;
float Cursor::dx = 0.0;
float Cursor::dy = 0.0;

float Cursor::lx = 0.0;
float Cursor::ly = 0.0;
bool Cursor::reset = false;
bool Cursor::firstFrame = true;
GLFWwindow* Cursor::window;

void Cursor::Init(GLFWwindow* w) {
	firstFrame = true;
	visible = reset = false;

	window = w;

	dx = dy = 0.0;
	lx = ly = 0.0;
	SetVisible(visible);
}

void Cursor::Update() {
	
}

void Cursor::PostUpdate() {
	dx = dy = 0.0;
}

void Cursor::MouseMoveCallback(float xPos, float yPos) {
	if (visible)  //dont move camera if cursor is visible
		return;

	if (!reset && !firstFrame) {
		dx += xPos - lx;
		dy += yPos - ly;
	}

	reset = firstFrame = false;
	lx = xPos;
	ly = yPos;
}

void Cursor::SetVisible(bool v) {
	if (v)
		reset = true;

	visible = v;
	glfwSetInputMode(window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}