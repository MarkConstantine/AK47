#pragma once

class Camera {
public:
    Camera(float window_width, float window_height, const ShaderProgram& shader_program);
    void Move(GLFWwindow* window);
private:
    void SetUniforms(const glm::vec3& direction, const glm::vec3& right);
private:
    float m_WindowWidth, m_WindowHeight, m_HorizontalAngle, m_VerticalAngle;
    glm::mat4 m_ModelMatrix, m_ViewMatrix, m_ProjectionMatrix;
    glm::vec3 m_Position, m_LightPosition;
    ShaderProgram m_ShaderProgram;
};