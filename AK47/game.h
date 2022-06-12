#pragma once
#include <memory>
#include <string>
#include <vector>

#include <gl/glew.h>
#include <glfw/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "camera.h"

class Game {
public:
    Game(int width, int height, const std::string& title);
    ~Game();
    void Run();
private:
    GLuint glInit();
    void FpsCounter() const;
private:
    int m_WindowWidth, m_WindowHeight;
    std::string m_WindowTitle;
    GLFWwindow* m_Window;
};