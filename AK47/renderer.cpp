#include "stdafx.h"
#include "renderer.h"

Renderer::Renderer(int window_width, int window_height)
    : m_ShaderProgram("vertex.glsl", "fragment.glsl")
    , m_AK47({ "ak47.obj", "AK47_BaseColor.png", "AK47_Normal.png", "AK47_Metallic.png" })
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
    auto program_id = m_ShaderProgram.GetProgramId();
    glUseProgram(program_id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_AK47.Render(program_id);
}