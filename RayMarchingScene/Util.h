#pragma once
#ifndef Util_H
#define Util_H
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Util {
public:
    Util();
    static void printShaderLog(GLuint shader);
    static void printProgramLog(int prog);
    static bool checkOpenGLError();
    static string readShaderSource(const char* filePath);
    static GLuint createShaderProgram(const char* vShaderName, const char* fShaderName);
    static GLuint createComputeShaderProgram(const char* cShaderName);
    static void CreateTextureImage(int w, int h, GLuint* texture);
    static void DetermineWorkGroupSize();
    static GLuint loadTexture(const char* texImagePath);
};

#endif