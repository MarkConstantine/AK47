#pragma once
#include <gl/glew.h>
#include <glfw/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include "drawable.h"
#include "camera.h"

class Game {
public:
    Game(int width, int height, std::string title);
    ~Game();
    void Run();
private:
    GLuint glInit();
    void FpsCounter() const;
    void Draw3D() const;
private:
    Camera* m_camera;
    int m_width;
    int m_height;
    std::string m_title;
    GLFWwindow* m_window;
    GLuint m_program_id;
    GLuint m_text_program_id;
    std::vector<Drawable*> m_drawables;
};