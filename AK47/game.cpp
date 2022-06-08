#include "game.h"
#include "shader.h"
#include "ak47.h"

Game::Game(int width, int height, std::string title)
{
    m_width = width;
    m_height = height;
    m_title = title;
    m_program_id = glInit();
    m_camera = new Camera((float)width, (float)height, m_program_id);
    m_drawables = std::vector<Drawable*>{
        new AK47(m_program_id)
    };
}

Game::~Game()
{
    for (auto drawables : m_drawables)
    {
        delete drawables;
    }
    delete m_camera;
    glDeleteProgram(m_program_id);
    glDeleteProgram(m_text_program_id);
    glfwTerminate();
}

void Game::Run()
{
    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);

    while (glfwGetKey(m_window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(m_window) == 0)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        FpsCounter();
        Draw3D();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

GLuint Game::glInit()
{
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW.\n");
        throw false;
    }

    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (m_window == nullptr)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        throw false;
    }

    glfwMakeContextCurrent(m_window);
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLFW.\n");
        throw false;
    }

    fprintf(stdout, "%s initialized\n", m_title.c_str());
    return LoadShaders("vertex.glsl", "fragment.glsl");
}

void Game::FpsCounter() const
{
    static auto previous_time = glfwGetTime();
    static auto frames = 0;

    auto current_time = glfwGetTime();
    frames++;
    if (current_time - previous_time >= 1.0)
    {
        auto ms_per_frame = 1000.0 / double(frames);
        char title[_MAX_PATH] = { 0 };
        sprintf_s(title, sizeof(title), "%s, %f ms/frame, FPS: %f", m_title.c_str(), ms_per_frame, 1.0 / (ms_per_frame / 1000.0));
        glfwSetWindowTitle(m_window, title);
        frames = 0;
        previous_time = current_time;
    }
}

void Game::Draw3D() const
{
    glUseProgram(m_program_id);
    m_camera->Move(m_window);
    for (auto drawables : m_drawables)
    {
        drawables->Draw();
    }
}
