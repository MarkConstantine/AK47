#pragma once

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();
    void Run();
private:
    void Init();
    void UpdateFpsCounter() const;
private:
    int m_WindowWidth, m_WindowHeight;
    std::string m_WindowTitle;
    GLFWwindow* m_Window;
};