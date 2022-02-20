#include "Input.h"

GLFWwindow* Input::win;
float Input::interpolationSpeed = 5.0;
float Input::forward = 0.0;
float Input::sideways = 0.0;
float Input::rawForward = 0.0;
float Input::rawSideways = 0.0;
float Input::mouseX = 0.0;
float Input::mouseY = 0.0;
float Input::rawMouseX = 0.0;
float Input::rawMouseY = 0.0;
bool Input::w = false;
bool Input::a = false;
bool Input::s = false;
bool Input::d = false;
bool Input::mouseLeftClick = false;
bool Input::mouseLeftHold = false;
bool Input::mouseRightClick = false;
bool Input::mouseRightHold = false;

void Input::Update(float deltaTime)
{
    UpdateTargets();
    forward = forward + ((rawForward - forward) * deltaTime * interpolationSpeed);
    sideways = sideways + ((rawSideways - sideways) * deltaTime * interpolationSpeed);
    mouseX = mouseX + ((rawMouseX - mouseX) * deltaTime * interpolationSpeed);
    mouseY = mouseY + ((rawMouseY - mouseY) * deltaTime * interpolationSpeed);
}

void Input::PostUpdate()
{
    mouseLeftClick = mouseRightClick = false;
}

void Input::UpdateTargets()
{
    rawForward = w ? 1 : (s ? -1 : 0);
    rawSideways = d ? 1 : (a ? -1 : 0);
    rawMouseX = Cursor::dx;
    rawMouseY = Cursor::dy;
}

void Input::KeyAction(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        w = key == GLFW_KEY_W ? true : w;
        a = key == GLFW_KEY_A ? true : a;
        s = key == GLFW_KEY_S ? true : s;
        d = key == GLFW_KEY_D ? true : d;

        if (key == GLFW_KEY_ESCAPE)
            Cursor::SetVisible(!Cursor::visible);
    }
    if (action == GLFW_RELEASE) {
        w = key == GLFW_KEY_W ? false : w;
        a = key == GLFW_KEY_A ? false : a;
        s = key == GLFW_KEY_S ? false : s;
        d = key == GLFW_KEY_D ? false : d;
    }
    
}

void Input::MouseButton(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
            mouseLeftClick = mouseLeftHold = true;
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
            mouseRightClick = mouseRightHold = true;
    }
    if (action == GLFW_RELEASE) {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
            mouseLeftHold = false;
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
            mouseRightHold = false;
    }
}

void Input::MouseMove(GLFWwindow* window, double xpos, double ypos)
{
    Cursor::MouseMoveCallback(xpos, ypos);
}

void Input::WindowReshape(GLFWwindow* window, int newWidth, int newHeight)
{

}
