#ifndef APPLICATION_H
#define APPLICATION_H

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#include <GLFW/glfw3.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "UIManager.h"

class Application
{
public:
    Application(GLFWwindow *window);
    bool running;
    void Render();
    void Cleanup();

private:
    GLFWwindow *m_window;
    int m_width;
    int m_height;
    const char *title;
    UIManager *m_ui;
};

#endif