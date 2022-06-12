#pragma once
#include "shader.h"
#include "mesh.h"

class Renderer
{
public:
    Renderer(int window_width, int window_height);
    GLuint GetProgramId();
    void Render();
private:
    ShaderProgram m_ShaderProgram;
    Mesh m_AK47;
};