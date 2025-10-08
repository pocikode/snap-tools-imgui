#include "../../include/platform/LinuxPlatform.h"

#ifdef __linux__
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include <GL/gl3w.h>

namespace Platform
{

    LinuxPlatform::LinuxPlatform()
        : m_window(nullptr), m_glContext(nullptr), m_imguiContext(nullptr), m_shouldClose(false)
    {
    }

    LinuxPlatform::~LinuxPlatform()
    {
        Shutdown();
    }

    bool LinuxPlatform::Initialize(const WindowConfig &config)
    {
        m_config = config;

        // Initialize SDL3
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMEPAD) != 0)
        {
            return false;
        }

        // Set OpenGL attributes
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        // Create window
        Uint32 window_flags = SDL_WINDOW_OPENGL;
        if (config.resizable)
        {
            window_flags |= SDL_WINDOW_RESIZABLE;
        }

        m_window = SDL_CreateWindow(config.title.c_str(), config.width, config.height, window_flags);
        if (!m_window)
        {
            return false;
        }

        // Create OpenGL context
        m_glContext = SDL_GL_CreateContext(m_window);
        if (!m_glContext)
        {
            return false;
        }

        SDL_GL_MakeCurrent(m_window, m_glContext);

        // Enable VSync
        if (config.vsync)
        {
            SDL_GL_SetSwapInterval(1);
        }

        // Initialize gl3w
        if (gl3wInit() != 0)
        {
            return false;
        }

        return true;
    }

    bool LinuxPlatform::InitializeImGui()
    {
        IMGUI_CHECKVERSION();
        m_imguiContext = ImGui::CreateContext();
        ImGui::SetCurrentContext(m_imguiContext);

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForOpenGL(m_window, m_glContext);
        ImGui_ImplOpenGL3_Init("#version 330 core");

        return true;
    }

    void LinuxPlatform::Shutdown()
    {
        ShutdownImGui();

        if (m_glContext)
        {
            SDL_GL_DeleteContext(m_glContext);
            m_glContext = nullptr;
        }

        if (m_window)
        {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }

        SDL_Quit();
    }

    bool LinuxPlatform::ShouldClose()
    {
        return m_shouldClose;
    }

    void LinuxPlatform::PollEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
            {
                m_shouldClose = true;
            }
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                event.window.windowID == SDL_GetWindowID(m_window))
            {
                m_shouldClose = true;
            }
        }
    }

    void LinuxPlatform::SwapBuffers()
    {
        SDL_GL_SwapWindow(m_window);
    }

    // ... Additional method implementations would go here ...

} // namespace Platform

#else
// Stub implementation for non-Linux platforms
namespace Platform
{
    LinuxPlatform::LinuxPlatform() {}
    LinuxPlatform::~LinuxPlatform() {}
    bool LinuxPlatform::Initialize(const WindowConfig &) { return false; }
    void LinuxPlatform::Shutdown() {}
    bool LinuxPlatform::ShouldClose() { return true; }
    void LinuxPlatform::PollEvents() {}
    void LinuxPlatform::SwapBuffers() {}
    void LinuxPlatform::SetWindowTitle(const std::string &) {}
    void LinuxPlatform::GetWindowSize(int &, int &) {}
    void LinuxPlatform::SetWindowSize(int, int) {}
    bool LinuxPlatform::InitializeRenderer() { return false; }
    void LinuxPlatform::ShutdownRenderer() {}
    void LinuxPlatform::NewFrame() {}
    void LinuxPlatform::RenderFrame() {}
    void LinuxPlatform::ClearBackground(float, float, float, float) {}
    bool LinuxPlatform::InitializeImGui() { return false; }
    void LinuxPlatform::ShutdownImGui() {}
    void LinuxPlatform::ImGuiNewFrame() {}
    void LinuxPlatform::ImGuiRender() {}
    void *LinuxPlatform::GetNativeWindow() { return nullptr; }
    void *LinuxPlatform::GetNativeRenderer() { return nullptr; }
}
#endif