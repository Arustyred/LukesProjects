#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Util.h"
#include "Transform.h"
#include "Camera.h"
#include "Cursor.h"
#include "Input.h"
#include "Player.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\type_ptr.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\string_cast.hpp>

using namespace std;

#define numVAOs 1
#define numVBOs 2
#define FRAMERATE 90  //currently not used (choppy if set to 60 for some reason)

int width = 1920;
int height = 1080;
int textureWidth = 1100; //480
int textureHeight = 620;  //270
double time, deltaTime;

GLuint renderTexture;
GLuint renderingProgram, computeProgram;

GLuint vao[numVAOs];
GLuint vbo[numVBOs];

GLuint aspectPtr, fovPtr;
GLuint cwmPtr, cpPtr;

Camera* cam;
Camera* Camera::activeCam;
Player* player;

float* verts = new float[8]{ -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0, -1.0 };
float* texCoords = new float[8]{ 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0 };

void init(GLFWwindow* window) {
    cam = new Camera(60.0f, width, height);
    player = new Player(2.75, 1.6);

    Cursor::Init(window);
    Input::interpolationSpeed = 10.0;

    Util::DetermineWorkGroupSize();
    Util::CreateTextureImage(textureWidth, textureHeight, &renderTexture);

    computeProgram = Util::createComputeShaderProgram("RayMarcher.glsl");
    renderingProgram = Util::createShaderProgram("VertShader.glsl", "FragShader.glsl");

    //Create vertex array object
    glGenVertexArrays(numVAOs, vao);
    glBindVertexArray(vao[0]);

    //Generate vertex buffer objects for vertices and texture coordinates
    glGenBuffers(numVBOs, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), texCoords, GL_STATIC_DRAW);
}

void display(GLFWwindow* window, double currentTime) {
    deltaTime = currentTime - time;
    time = currentTime;

    Cursor::Update();
    Input::Update(deltaTime);
    player->Update(deltaTime);
    //cout << Input::rawMouseX << " " << Input::rawMouseY << endl;

    glUseProgram(computeProgram);

    //get pointers to compute shader program
    cwmPtr = glGetUniformLocation(computeProgram, "camWorldMatrix");
    cpPtr = glGetUniformLocation(computeProgram, "camPos");
    aspectPtr = glGetUniformLocation(computeProgram, "aspectRatio");
    fovPtr = glGetUniformLocation(computeProgram, "fovFactor");

    //set the values of the pointers
    glUniformMatrix4fv(cwmPtr, 1, GL_FALSE, glm::value_ptr(cam->vMat));
    glProgramUniform4fv(computeProgram, cpPtr, 1, glm::value_ptr(glm::vec4(cam->GetPosition(), 1.0)));
    glProgramUniform1f(computeProgram, aspectPtr, cam->apectRatio);
    glProgramUniform1f(computeProgram, fovPtr, cam->fovScale);

    //Execute the compute shader (It writes to an image texture)
    glDispatchCompute((GLuint)textureWidth, (GLuint)textureHeight, 1);

    //ensure compute shader has completed writing to image before continuing
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    //Clear the screen color and switch to the default rendering program
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(renderingProgram);

    //bind vertices to the first buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    //bind texture coordinates to second buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    //set the active texture to the completed compute shader image
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderTexture);

    //draw the image
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    Input::PostUpdate();
    Cursor::PostUpdate();
}

void keyPressed(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Input::KeyAction(window, key, scancode, action, mods);
}

void mouseMove(GLFWwindow* window, double xpos, double ypos) {
    Input::MouseMove(window, xpos, ypos);
}

void mouseButton(GLFWwindow* window, int button, int action, int mods) {
    Input::MouseButton(window, button, action, mods);
}

void WindowReshapeCallback(GLFWwindow* window, int newWidth, int newHeight) {
    Input::WindowReshape(window, newWidth, newHeight);
    glViewport(0, 0, newWidth, newHeight);
}


int main(void)
{
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* window = glfwCreateWindow(width, height, "Luke's Window", NULL, NULL);
    glfwMakeContextCurrent(window);


    if (glewInit() != GLEW_OK) {
        exit(EXIT_FAILURE);
    }

    glfwSwapInterval(1);
    init(window);

    glfwSetKeyCallback(window, keyPressed);
    glfwSetMouseButtonCallback(window, mouseButton);
    glfwSetWindowSizeCallback(window, WindowReshapeCallback);
    glfwSetCursorPosCallback(window, mouseMove);


    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}