#include <fstream>
#include <sstream>
#include <vector>

#include "shader.h"

GLuint LoadShaderFromFile(const std::string& shader_file, GLint shader_type)
{
    auto shader_id = glCreateShader(shader_type);

    std::ifstream shader_stream(shader_file, std::ios::in);
    if (!shader_stream.is_open())
    {
        fprintf(stderr, "Failed to open %s\n", shader_file.c_str());
        throw false;
    }

    std::stringstream sstr;
    sstr << shader_stream.rdbuf();
    auto shader_code = sstr.str();
    shader_stream.close();

    const char* shader_code_ptr = shader_code.c_str();
    glShaderSource(shader_id, 1, &shader_code_ptr, NULL);
    glCompileShader(shader_id);
    fprintf(stdout, "Compiled shader: %s\n", shader_file.c_str());

    GLint compile_status = GL_FALSE;
    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 0)
    {
        std::vector<char> error_message(log_length + 1);
        glGetShaderInfoLog(shader_id, log_length, NULL, &error_message[0]);
        fprintf(stdout, "Compile Status: %d, Shader Info Log: %s\n", compile_status, &error_message[0]);
    }

    return shader_id;
}

GLuint LoadShaders(const std::string& vertex_shader, const std::string& fragment_shader)
{
    auto vertex_shader_id = LoadShaderFromFile(vertex_shader, GL_VERTEX_SHADER);
    auto fragment_shader_id = LoadShaderFromFile(fragment_shader, GL_FRAGMENT_SHADER);

    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);
    fprintf(stdout, "Linked program\n");

    GLint link_status = 0;
    GLint log_length = 0;
    glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 0)
    {
        std::vector<char> error_message(log_length + 1);
        glGetProgramInfoLog(program_id, log_length, NULL, &error_message[0]);
        fprintf(stdout, "Link Status: %d. Program Info Log: %s\n", link_status, &error_message[0]);
    }

    glDetachShader(program_id, vertex_shader_id);
    glDetachShader(program_id, fragment_shader_id);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    fprintf(stdout, "Shaders loaded. Program ID: %d\n", program_id);
    return program_id;
}
