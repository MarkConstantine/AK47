#pragma once

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
    void UseProgram() const;
    void SetUniform(const std::string& name, const glm::mat4& value);
    void SetUniform(const std::string& name, const glm::vec3& value);
    void SetUniform(const std::string& name, int value);
private:
    GLuint m_ProgramId;
    Shader m_VertexShader;
    Shader m_FragmentShader;
};