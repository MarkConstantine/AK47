#include "stdafx.h"
#include "renderer.h"

Renderer::Renderer(int window_width, int window_height)
    : m_ShaderProgram("vertex.glsl", "fragment.glsl")
    , m_AK47("ak47.obj")
{
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
}

GLuint Renderer::GetProgramId()
{
    return m_ShaderProgram.GetProgramId();
}

void Renderer::Render()
{
    glUseProgram(m_ShaderProgram.GetProgramId());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_AK47.Render();
}