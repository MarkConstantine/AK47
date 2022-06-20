#pragma once

class Camera {
public:
    Camera(float width, float height, GLuint program_id);
    void Move(GLFWwindow* window);
private:
    void SetUniforms(const glm::vec3& direction, const glm::vec3& right);
private:
    float m_WindowWidth, m_WindowHeight, m_FOV;
    float m_HorizontalAngle, m_VerticalAngle, m_DefaultMoveSpeed, m_FastMoveSpeed, m_MouseSpeed;
    float m_LightPower;
    glm::mat4 m_ModelMatrix, m_ViewMatrix, m_ProjectionMatrix;
    glm::vec3 m_Position, m_LightPosition, m_LightColor;
    GLuint m_ModelUniform, m_ViewUniform, m_ProjectionUniform, m_CameraPositionUniform;
    GLuint m_LightPositionUniform, m_LightColorUniform, m_LightPowerUniform;
};