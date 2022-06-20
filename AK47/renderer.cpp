#include "stdafx.h"
#include "renderer.h"
#include "shader.h"
#include "camera.h"
#include "mesh.h"

Renderer::Renderer(int window_width, int window_height)
    : m_ShaderProgram("vertex.glsl", "fragment.glsl")
    , m_Camera((float)window_width, (float)window_height, m_ShaderProgram)
    , m_AK47({ "ak47.obj", "AK47_BaseColor.png", "AK47_Normal.png", "AK47_Metallic.png" })
{
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
}

void Renderer::Render(GLFWwindow* window)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_ShaderProgram.UseProgram();
    m_Camera.Move(window);
    m_AK47.Render(m_ShaderProgram);
}