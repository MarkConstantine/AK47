#include "stdafx.h"
#include "window.h"
#include "renderer.h"
#include "camera.h"

Window::Window(int width, int height, const std::string& title)
{
    m_WindowWidth = width;
    m_WindowHeight = height;
    m_WindowTitle = title;
}

Window::~Window()
{
    glfwTerminate();
}

void Window::Run()
{
    Init();
    Renderer renderer(m_WindowWidth, m_WindowHeight);
    Camera camera((float)m_WindowWidth, (float)m_WindowHeight, renderer.GetProgramId());

    while (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(m_Window) == 0)
    {
        UpdateFpsCounter();

        camera.Move(m_Window);
        renderer.Render();

        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }
}

void Window::Init()
{
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW.\n");
        exit(EXIT_FAILURE);
    }

    m_Window = glfwCreateWindow(m_WindowWidth, m_WindowHeight, m_WindowTitle.c_str(), nullptr, nullptr);
    glViewport(0, 0, m_WindowWidth, m_WindowHeight);
    if (m_Window == nullptr)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(m_Window);
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLFW.\n");
        exit(EXIT_FAILURE);
    }

    glfwSetInputMode(m_Window, GLFW_STICKY_KEYS, GL_TRUE);
    
    fprintf(stdout, "%s initialized\n", m_WindowTitle.c_str());
}

void Window::UpdateFpsCounter() const
{
    static auto previous_time = glfwGetTime();
    static auto frames = 0;

    auto current_time = glfwGetTime();
    frames++;
    if (current_time - previous_time >= 1.0)
    {
        auto ms_per_frame = 1000.0 / double(frames);
        char title[_MAX_PATH] = { 0 };
        sprintf_s(title, sizeof(title), "%s, %f ms/frame, FPS: %f",
            m_WindowTitle.c_str(), ms_per_frame, 1.0 / (ms_per_frame / 1000.0));
        glfwSetWindowTitle(m_Window, title);
        frames = 0;
        previous_time = current_time;
    }
}
