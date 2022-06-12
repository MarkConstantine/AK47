#pragma once
#include <GL/glew.h>
#include <string>
#include <memory>

class Shader
{
public:
    Shader(const std::string& file_path, GLint shader_type);
    ~Shader();
    GLuint GetShaderId() const;
    GLint GetShaderType() const;
private:
    GLuint m_ShaderId;
    GLint m_ShaderType;
};

class ShaderProgram
{
public:
    ShaderProgram(const std::string& vertex_shader_file, const std::string& fragment_shader_file);
    ~ShaderProgram();
    GLuint GetProgramId() const;
    Shader GetVertexShader() const;
    Shader GetFragmentShader() const;
private:
    GLuint m_ProgramId;
    Shader m_VertexShader;
    Shader m_FragmentShader;
};