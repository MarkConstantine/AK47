#define _USE_MATH_DEFINES
#include <cmath>
#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "camera.h"

Camera::Camera(float width, float height, GLuint program_id)
{
    m_width = width;
    m_height = height;
    m_horizontal_angle = M_PI;
    m_vertical_angle = 0.f;
    m_fov = 45.f;
    m_default_move_speed = 10.f;
    m_fast_move_speed = 3 * m_default_move_speed;
    m_mouse_speed = 0.005f;
    m_position = glm::vec3(0, 0, 5);
    m_model_matrix = glm::mat4(1.f);
    m_view_matrix = glm::lookAt(
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0)
    );
    m_light_position = glm::vec3(1, 1, 5);
    m_light_color = glm::vec3(1, 1, 1);
    m_light_power = 50.f;
    m_projection_matrix = glm::perspective(glm::radians(m_fov), width / height, 0.1f, 100.0f);
    m_model_id = glGetUniformLocation(program_id, "model");
    m_view_id = glGetUniformLocation(program_id, "view");
    m_projection_id = glGetUniformLocation(program_id, "projection");
    m_light_id = glGetUniformLocation(program_id, "lightPosition_worldspace");
    m_light_color_id = glGetUniformLocation(program_id, "lightColor");
    m_light_power_id = glGetUniformLocation(program_id, "lightPower");
}

void Camera::Move(GLFWwindow* window)
{
    static auto last_time = glfwGetTime(); // Called only once on initialization!
    auto current_time = glfwGetTime();

    auto dt = float(current_time - last_time);

    double xpos = 0, ypos = 0;
    glfwGetCursorPos(window, &xpos, &ypos);
    glfwSetCursorPos(window, m_width / 2, m_height / 2);

    m_horizontal_angle += m_mouse_speed * float(m_width / 2 - xpos);
    m_vertical_angle += m_mouse_speed * float(m_height / 2 - ypos);

    if (m_vertical_angle > 1.5f)
        m_vertical_angle = 1.5f;
    if (m_vertical_angle < -1.5f)
        m_vertical_angle = -1.5f;

    auto direction = glm::vec3(
        cos(m_vertical_angle) * sin(m_horizontal_angle),
        sin(m_vertical_angle),
        cos(m_vertical_angle) * cos(m_horizontal_angle)
    );

    auto right = glm::vec3(
        sin(m_horizontal_angle - M_PI / 2.f),
        0,
        cos(m_horizontal_angle - M_PI / 2.f)
    );

    auto move_speed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? m_fast_move_speed : m_default_move_speed;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_position += direction * dt * move_speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_position -= direction * dt * move_speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_position += right * dt * move_speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_position -= right * dt * move_speed;

    last_time = current_time;

    GetMvp(direction, right);
}

void Camera::GetMvp(const glm::vec3& direction, const glm::vec3& right)
{
    glm::vec3 up = glm::cross(right, direction);

    m_projection_matrix = glm::perspective(glm::radians(m_fov), m_width / m_height, 0.1f, 100.0f);
    glUniformMatrix4fv(m_projection_id, 1, GL_FALSE, &m_projection_matrix[0][0]);

    m_view_matrix = glm::lookAt(
        m_position,
        m_position + direction,
        up
    );
    glUniformMatrix4fv(m_view_id, 1, GL_FALSE, &m_view_matrix[0][0]);

    glUniformMatrix4fv(m_model_id, 1, GL_FALSE, &m_model_matrix[0][0]);

    glUniform3f(m_light_id, m_light_position.x, m_light_position.y, m_light_position.z);
    glUniform3f(m_light_color_id, m_light_color.x, m_light_color.y, m_light_color.z);
    glUniform1f(m_light_power_id, m_light_power);
}