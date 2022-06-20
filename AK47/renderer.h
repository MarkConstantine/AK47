#pragma once
#include "shader.h"
#include "camera.h"
#include "mesh.h"

class Renderer
{
public:
    Renderer(int window_width, int window_height);
    void Render(GLFWwindow* window);
private:
    ShaderProgram m_ShaderProgram;
    Camera m_Camera;
    Mesh m_AK47;
};