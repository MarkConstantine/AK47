#include "stdafx.h"
#include "camera.h"

Camera::Camera(float width, float height, GLuint program_id)
{
    m_WindowWidth = width;
    m_WindowHeight = height;
    m_HorizontalAngle = M_PI;
    m_VerticalAngle = 0.f;
    m_FOV = 45.f;
    m_DefaultMoveSpeed = 0.5f;
    m_FastMoveSpeed = 3 * m_DefaultMoveSpeed;
    m_MouseSpeed = 0.0005f;
    m_Position = glm::vec3(0, 0, 1);
    m_ModelMatrix = glm::mat4(1.f);
    m_ViewMatrix = glm::lookAt(
        m_Position,
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0)
    );
    m_LightPosition = glm::vec3(1, 1, 5);
    m_LightColor = glm::vec3(1, 1, 1);
    m_LightPower = 20.f;
    m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), width / height, 0.1f, 100.0f);
    m_ModelUniform = glGetUniformLocation(program_id, "model");
    m_ViewUniform = glGetUniformLocation(program_id, "view");
    m_ProjectionUniform = glGetUniformLocation(program_id, "projection");
    m_LightPositionUniform = glGetUniformLocation(program_id, "lightPosition_worldspace");
    m_LightColorUniform = glGetUniformLocation(program_id, "lightColor");
    m_LightPowerUniform = glGetUniformLocation(program_id, "lightPower");
}

void Camera::Move(GLFWwindow* window)
{
    static auto last_time = glfwGetTime(); // Called only once on initialization!
    auto current_time = glfwGetTime();

    auto dt = float(current_time - last_time);

    double xpos = 0, ypos = 0;
    glfwGetCursorPos(window, &xpos, &ypos);
    glfwSetCursorPos(window, m_WindowWidth / 2, m_WindowHeight / 2);

    m_HorizontalAngle += m_MouseSpeed * float(m_WindowWidth / 2 - xpos);
    m_VerticalAngle += m_MouseSpeed * float(m_WindowHeight / 2 - ypos);

    if (m_VerticalAngle > 1.5f)
        m_VerticalAngle = 1.5f;
    if (m_VerticalAngle < -1.5f)
        m_VerticalAngle = -1.5f;

    auto direction = glm::vec3(
        cos(m_VerticalAngle) * sin(m_HorizontalAngle),
        sin(m_VerticalAngle),
        cos(m_VerticalAngle) * cos(m_HorizontalAngle)
    );

    auto right = glm::vec3(
        sin(m_HorizontalAngle - M_PI / 2.f),
        0,
        cos(m_HorizontalAngle - M_PI / 2.f)
    );

    auto move_speed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) 
        ? m_FastMoveSpeed : m_DefaultMoveSpeed;
    
    
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        m_Position.y += dt * move_speed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        m_Position.y -= dt * move_speed;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_Position += direction * dt * move_speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_Position -= direction * dt * move_speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_Position += right * dt * move_speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_Position -= right * dt * move_speed;

    last_time = current_time;

    GetMvp(direction, right);
}

void Camera::GetMvp(const glm::vec3& direction, const glm::vec3& right)
{
    glm::vec3 up = glm::cross(right, direction);

    m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_WindowWidth / m_WindowHeight, 0.1f, 100.0f);
    m_ViewMatrix = glm::lookAt(
        m_Position,
        m_Position + direction,
        up
    );

    glUniformMatrix4fv(m_ProjectionUniform, 1, GL_FALSE, &m_ProjectionMatrix[0][0]);
    glUniformMatrix4fv(m_ViewUniform, 1, GL_FALSE, &m_ViewMatrix[0][0]);
    glUniformMatrix4fv(m_ModelUniform, 1, GL_FALSE, &m_ModelMatrix[0][0]);

    glUniform3f(m_LightPositionUniform, m_LightPosition.x, m_LightPosition.y, m_LightPosition.z);
    glUniform3f(m_LightColorUniform, m_LightColor.x, m_LightColor.y, m_LightColor.z);
    glUniform1f(m_LightPowerUniform, m_LightPower);
}