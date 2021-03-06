#include "stdafx.h"
#include "shader.h"
#include "util.h"

Shader::Shader(const std::string& filename, GLint shader_type)
{
    m_ShaderType = shader_type;
    m_ShaderId = glCreateShader(m_ShaderType);
    
    std::string absolute_path = util::GetShaderFilePath(filename).string();
    std::ifstream shader_stream(absolute_path, std::ios::in);
    if (!shader_stream.is_open())
    {
        fprintf(stderr, "Failed to open %s\n", absolute_path.c_str());
        exit(EXIT_FAILURE);
    }

    std::stringstream sstr;
    sstr << shader_stream.rdbuf();
    std::string shader_code = sstr.str();
    shader_stream.close();

    const char* shader_code_ptr = shader_code.c_str();
    glShaderSource(m_ShaderId, 1, &shader_code_ptr, NULL);
    glCompileShader(m_ShaderId);
    fprintf(stdout, "Compiled Shader: %s\n", absolute_path.c_str());

    GLint compile_status = GL_FALSE;
    GLint log_length = 0;
    glGetShaderiv(m_ShaderId, GL_COMPILE_STATUS, &compile_status);
    glGetShaderiv(m_ShaderId, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 0)
    {
        std::vector<char> error_message(size_t(log_length) + 1);
        glGetShaderInfoLog(m_ShaderId, log_length, NULL, &error_message[0]);
        fprintf(stderr, "Compile Status: %d, Shader Info Log: %s\n", compile_status, &error_message[0]);
        exit(EXIT_FAILURE);
    }
}

Shader::~Shader()
{
    glDeleteShader(m_ShaderId);
}


GLuint Shader::GetShaderId() const
{
    return m_ShaderId;
}

GLint Shader::GetShaderType() const
{
    return m_ShaderType;
}

ShaderProgram::ShaderProgram(const std::string& vertex_shader_filename, const std::string& fragment_shader_filename)
    : m_VertexShader(Shader(vertex_shader_filename, GL_VERTEX_SHADER))
    , m_FragmentShader(Shader(fragment_shader_filename, GL_FRAGMENT_SHADER))
{
    m_ProgramId = glCreateProgram();
    glAttachShader(m_ProgramId, m_VertexShader.GetShaderId());
    glAttachShader(m_ProgramId, m_FragmentShader.GetShaderId());
    glLinkProgram(m_ProgramId);
    fprintf(stdout, "Linked programs %d %d\n", m_VertexShader.GetShaderId(), m_FragmentShader.GetShaderId());

    GLint link_status = 0;
    GLint log_length = 0;
    glGetProgramiv(m_ProgramId, GL_LINK_STATUS, &link_status);
    glGetProgramiv(m_ProgramId, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 0)
    {
        std::vector<char> error_message(size_t(log_length) + 1, '\0');
        glGetProgramInfoLog(m_ProgramId, log_length, nullptr, &error_message[0]);
        fprintf(stderr, "Link Status: %d\nProgram Info Log: %s\n", link_status, error_message.data());
    }

    glDetachShader(m_ProgramId, m_VertexShader.GetShaderId());
    glDetachShader(m_ProgramId, m_FragmentShader.GetShaderId());

    fprintf(stdout, "Shaders loaded. Program ID: %d\n", m_ProgramId);
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_ProgramId);
}

GLuint ShaderProgram::GetProgramId() const
{
    return m_ProgramId;
}

Shader ShaderProgram::GetVertexShader() const
{
    return m_VertexShader;
}

Shader ShaderProgram::GetFragmentShader() const
{
    return m_FragmentShader;
}

void ShaderProgram::UseProgram() const
{
    glUseProgram(m_ProgramId);
}

void ShaderProgram::SetUniform(const std::string& name, const glm::mat4& value)
{
    auto location = glGetUniformLocation(m_ProgramId, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

void ShaderProgram::SetUniform(const std::string& name, const glm::vec3& value)
{
    auto location = glGetUniformLocation(m_ProgramId, name.c_str());
    glUniform3f(location, value.x, value.y, value.z);
}

void ShaderProgram::SetUniform(const std::string& name, int value)
{
    auto location = glGetUniformLocation(m_ProgramId, name.c_str());
    glUniform1i(location, value);
}
