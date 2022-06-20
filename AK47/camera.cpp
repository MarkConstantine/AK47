#include "stdafx.h"
#include "shader.h"
#include "camera.h"

const auto kFOV = 45.f;
const auto kMouseSpeed = 0.0005f;
const auto kMoveSpeed = 0.5f;
const auto kMoveSpeedFast = 3.f * kMoveSpeed;

Camera::Camera(float window_width, float window_height, const ShaderProgram& shader_program)
    : m_ShaderProgram(shader_program)
{
    m_WindowWidth = window_width;
    m_WindowHeight = window_height;
    m_HorizontalAngle = M_PI;
    m_VerticalAngle = 0.f;
    m_Position = glm::vec3(0, 0, 1);
    m_ModelMatrix = glm::mat4(1.f);
    m_ViewMatrix = glm::lookAt(
        m_Position,
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0)
    );
    m_LightPosition = m_Position;
    m_ProjectionMatrix = glm::perspective(glm::radians(kFOV), window_width / window_height, 0.1f, 100.0f);
}

void Camera::Move(GLFWwindow* window)
{
    static auto last_time = glfwGetTime(); // Called only once on initialization!
    auto current_time = glfwGetTime();

    auto dt = float(current_time - last_time);

    double xpos = 0, ypos = 0;
    glfwGetCursorPos(window, &xpos, &ypos);
    glfwSetCursorPos(window, m_WindowWidth / 2, m_WindowHeight / 2);

    auto move_speed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? kMoveSpeedFast : kMoveSpeed;

    m_HorizontalAngle += kMouseSpeed * float(m_WindowWidth / 2 - xpos);
    m_VerticalAngle += kMouseSpeed * float(m_WindowHeight / 2 - ypos);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_HorizontalAngle += dt * move_speed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_HorizontalAngle -= dt * move_speed;

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
        sin(m_HorizontalAngle - M_PI_2),
        0,
        cos(m_HorizontalAngle - M_PI_2)
    );

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

    m_LightPosition = m_Position;

    SetUniforms(direction, right);
}

void Camera::SetUniforms(const glm::vec3& direction, const glm::vec3& right)
{
    glm::vec3 up = glm::cross(right, direction);

    m_ProjectionMatrix = glm::perspective(glm::radians(kFOV), m_WindowWidth / m_WindowHeight, 0.1f, 100.0f);
    m_ViewMatrix = glm::lookAt(
        m_Position,
        m_Position + direction,
        up
    );

    m_ShaderProgram.SetUniform("model", m_ModelMatrix);
    m_ShaderProgram.SetUniform("view", m_ViewMatrix);
    m_ShaderProgram.SetUniform("projection", m_ProjectionMatrix);
    m_ShaderProgram.SetUniform("cameraPosition_worldspace", m_Position);
    m_ShaderProgram.SetUniform("lightPosition_worldspace", m_LightPosition);
}