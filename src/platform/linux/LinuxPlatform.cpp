#include "platform/LinuxPlatform.h"
#include "imgui.h"
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_video.h>
#include <iostream>
#include <ostream>

#ifdef __linux__
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"

namespace Platform
{
    LinuxPlatform::LinuxPlatform() : m_window(nullptr), m_glContext(nullptr), m_imguiContext(nullptr), m_io(nullptr), m_shouldClose(false) {}

    LinuxPlatform::~LinuxPlatform() { Shutdown(); }

    bool LinuxPlatform::Initialize(const WindowConfig &config)
    {
        m_config = config;

        // Initialize SDL3
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
        {
            std::cout << "Failed to init SDL" << std::endl;
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
        float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

        SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL;
        if (config.resizable)
        {
            window_flags |= SDL_WINDOW_RESIZABLE;
        }
        window_flags |= SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;

        m_window = SDL_CreateWindow(config.title.c_str(), (int)(config.width * main_scale), (int)(config.height * main_scale), window_flags);
        if (m_window == nullptr)
        {
            std::cout << "Failed to create SDL window" << std::endl;
            return false;
        }

        if (!InitializeRenderer())
        {
            return false;
        }
        if (!InitializeImGui())
        {
            return false;
        }

        // Enable VSync
        if (config.vsync)
        {
            SDL_GL_SetSwapInterval(1);
        }
        SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(m_window);

        return true;
    }

    bool LinuxPlatform::InitializeRenderer()
    {
        // Create OpenGL context
        m_glContext = SDL_GL_CreateContext(m_window);
        if (m_glContext == nullptr)
        {
            std::cout << "Failed to create SDL renderer" << std::endl;
            return false;
        }

        SDL_GL_MakeCurrent(m_window, m_glContext);

        return true;
    }

    bool LinuxPlatform::InitializeImGui()
    {
        IMGUI_CHECKVERSION();
        m_imguiContext = ImGui::CreateContext();
        ImGui::SetCurrentContext(m_imguiContext);

        ImGuiIO &m_io = ImGui::GetIO();
        m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForOpenGL(m_window, m_glContext);
        ImGui_ImplOpenGL3_Init("#version 330 core");

        return true;
    }

    void LinuxPlatform::Shutdown()
    {
        ShutdownImGui();
        ShutdownRenderer();

        if (m_window)
        {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }

        SDL_Quit();
    }

    void LinuxPlatform::ShutdownRenderer()
    {
        if (m_glContext)
        {
            SDL_GL_DestroyContext(m_glContext);
        }
    }

    void LinuxPlatform::ShutdownImGui()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }

    bool LinuxPlatform::ShouldClose() { return m_shouldClose; }

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
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(m_window);
    }

    void LinuxPlatform::NewFrame() {}

    void LinuxPlatform::RenderFrame() {}

    void LinuxPlatform::ImGuiNewFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    void LinuxPlatform::ImGuiRender()
    {
        ImGui::Render();
        // glClearColor();
        // glClear(GL_COLOR_BUFFER_BIT);
    }

    void LinuxPlatform::ClearBackground(float r, float g, float b, float a)
    {
        glViewport(0, 0, (int)m_io->DisplaySize.x, (int)m_io->DisplaySize.y);
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void LinuxPlatform::SetWindowTitle(const std::string &title)
    {
        SDL_SetWindowTitle(m_window, title.c_str());
    }

    void LinuxPlatform::GetWindowSize(int &width, int &height)
    {
        SDL_GetWindowSize(m_window, &width, &height);
    }

    void LinuxPlatform::SetWindowSize(int width, int height)
    {
        SDL_SetWindowSize(m_window, width, height);
    }

    void *LinuxPlatform::GetNativeWindow()
    {
        return m_window;
    }

    void *LinuxPlatform::GetNativeRenderer()
    {
        return nullptr;
    }

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
} // namespace Platform
#endif
