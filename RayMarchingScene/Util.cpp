#include "Util.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "SOIL2/SOIL2.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Util::Util() {}

void Util::printShaderLog(GLuint shader) {
    int len = 0;
    int cw = 0; //characters written
    char* log;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        log = (char*)malloc(len);
        glGetShaderInfoLog(shader, len, &cw, log);
        cout << "Shader info log: " << log << endl;
        free(log);
    }
}

void Util::printProgramLog(int prog) {
    int len = 0;
    int cw = 0; //characters written
    char* log;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        log = (char*)malloc(len);
        glGetProgramInfoLog(prog, len, &cw, log);
        cout << "Program info log: " << log << endl;
        free(log);
    }
}

bool Util::checkOpenGLError() {
    bool foundError = false;
    int glErr = glGetError();
    while (glErr != GL_NO_ERROR) {
        cout << "glError: " << glErr << endl;
        foundError = true;
        glErr = glGetError();
    }
    return foundError;
}

string Util::readShaderSource(const char* filePath) {
    string content;
    ifstream fileStream(filePath, ios::in);
    string line = "";

    while (!fileStream.eof()) {
        getline(fileStream, line);
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}

GLuint Util::createShaderProgram(const char* vShaderName, const char* fShaderName) {
    GLint vertCompiled;
    GLint fragCompiled;
    GLint linked;

    string verShaderStr = readShaderSource(vShaderName);
    string fragShaderStr = readShaderSource(fShaderName);
    const char* vshaderSource = verShaderStr.c_str();
    const char* fshaderSource = fragShaderStr.c_str();

    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);  //tell it to create a vertex shader and fragment shader (initially empty)
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);  //returns an integer id for each shader

    //second parameter is the number of strings in the shader source code
    //if thats greater than 1, the third paramater has to be a pointer to an array of strings
    glShaderSource(vShader, 1, &vshaderSource, NULL);  //loads GLSL code from the strings into the empty objects
    glShaderSource(fShader, 1, &fshaderSource, NULL);

    //compile shaders
    glCompileShader(vShader);
    checkOpenGLError();
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
    if (vertCompiled != 1) {
        cout << "Vert compilation failed" << endl;
        printShaderLog(vShader);
    }

    glCompileShader(fShader);
    checkOpenGLError();
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
    if (fragCompiled != 1) {
        cout << "Frag compilation failed" << endl;
        printShaderLog(fShader);
    }

    GLuint vfProgram = glCreateProgram();  //creates a program, which is a series of compiled shaders
    glAttachShader(vfProgram, vShader);   //attach shaders
    glAttachShader(vfProgram, fShader);
    glLinkProgram(vfProgram);  //request the GLSL compiler ensure the shaders are compatable
    checkOpenGLError();
    glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
    if (linked != 1) {
        cout << "Link failed" << endl;
        printProgramLog(vfProgram);
    }

    return vfProgram;
}

GLuint Util::createComputeShaderProgram(const char* cShaderName) {
    GLint compiled;
    GLint linked;

    string cShaderStr = readShaderSource(cShaderName);
    const char* cshaderSource = cShaderStr.c_str();

    GLuint cShader = glCreateShader(GL_COMPUTE_SHADER);  //create a compute shader object
    glShaderSource(cShader, 1, &cshaderSource, NULL);  //loads GLSL code from the strings into the empty objects

    //compile shader
    glCompileShader(cShader);
    checkOpenGLError();
    glGetShaderiv(cShader, GL_COMPILE_STATUS, &compiled);
    if (compiled != 1) {
        cout << "Vert compilation failed" << endl;
        printShaderLog(cShader);
    }

    GLuint cProgram = glCreateProgram();  //creates a program, which is a series of compiled shaders
    glAttachShader(cProgram, cShader);   //attach shader
    glLinkProgram(cProgram);  //request the GLSL compiler ensure the shaders are compatable
    checkOpenGLError();
    glGetProgramiv(cProgram, GL_LINK_STATUS, &linked);
    if (linked != 1) {
        cout << "Link failed" << endl;
        printProgramLog(cProgram);
    }

    return cProgram;
}

void Util::CreateTextureImage(int w, int h, GLuint* texture) {
    glGenTextures(1, texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, *texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void Util::DetermineWorkGroupSize() {
    int workGroupCount[3];
    int workGroupSize[3];
    int workGroupInv;

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCount[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCount[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCount[2]);

    cout << "Max global (total) work group counts x: " << workGroupCount[0] << " y: "
        << workGroupCount[1] << " z: " << workGroupCount[2] << endl;

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize[2]);

    cout << "Max local (in one shader) work group counts x: " << workGroupSize[0] << " y: "
        << workGroupSize[1] << " z: " << workGroupSize[2] << endl;


    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workGroupInv);
    cout << "Max local work group invocations: " << workGroupInv << endl;
}

GLuint Util::loadTexture(const char* texImagePath) {
    GLuint textureID;
    textureID = SOIL_load_OGL_texture(texImagePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    if (textureID == 0)
        cout << "Could not find texture file " << texImagePath << endl;
    return textureID;
}