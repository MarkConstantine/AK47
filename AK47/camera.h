#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(float width, float height, GLuint program_id);
    void Move(GLFWwindow* window);
private:
    void GetMvp(const glm::vec3& direction, const glm::vec3& right);
private:
    float m_width;
    float m_height;
    float m_horizontal_angle;
    float m_vertical_angle;
    float m_fov;
    float m_default_move_speed, m_fast_move_speed, m_mouse_speed;
    glm::vec3 m_position;
    glm::mat4 m_model_matrix;
    glm::mat4 m_view_matrix;
    glm::mat4 m_projection_matrix;
    glm::vec3 m_light_position;
    glm::vec3 m_light_color;
    float m_light_power;
    GLuint m_mvp_id;
    GLuint m_model_id;
    GLuint m_view_id;
    GLuint m_projection_id;
    GLuint m_light_id;
    GLuint m_light_color_id;
    GLuint m_light_power_id;
};