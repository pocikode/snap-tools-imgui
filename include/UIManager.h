#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

class UIManager
{
public:
    UIManager(GLFWwindow *window);
    ~UIManager();
    void Cleanup();
    void RenderExample();

private:
    ImGuiContext *m_context;
    ImGuiIO *m_io;
};