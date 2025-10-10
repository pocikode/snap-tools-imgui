#include "platform/LinuxPlatform.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <iostream>

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
            std::cout << "Error: SDL_Init(): " << SDL_GetError() << std::endl;
            return false;
        }

        // GL 3.0 + GLSL 130
        m_glslVersion = const_cast<char *>("#version 130");
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

        // Create window
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
        SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
        m_window = SDL_CreateWindow(config.title.c_str(), (int)(config.width * main_scale), (int)(config.height * main_scale), window_flags);
        if (m_window == nullptr)
        {
            std::cout << "Error: SDL_CreateWindow(): " << SDL_GetError() << std::endl;
            return false;
        }

        if (!InitializeRenderer())
        {
            return false;
        };

        if (!InitializeImGui())
        {
            return false;
        };

        return true;
    }

    bool LinuxPlatform::InitializeRenderer()
    {
        m_glContext = SDL_GL_CreateContext(m_window);
        if (m_glContext == nullptr)
        {
            std::cout << "Error: SDL_GL_CreateContext(): " << SDL_GetError() << std::endl;
            return false;
        }

        SDL_GL_MakeCurrent(m_window, m_glContext);
        SDL_GL_SetSwapInterval(1); // Enable vsync
        SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(m_window);

        return true;
    }

    bool LinuxPlatform::InitializeImGui()
    {
        IMGUI_CHECKVERSION();
        m_imguiContext = ImGui::CreateContext();
        ImGui::SetCurrentContext(m_imguiContext);

        m_io = &ImGui::GetIO();
        m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        float main_scale = SDL_GetWindowDisplayScale(m_window);

        ImGui::StyleColorsDark();

        m_style = &ImGui::GetStyle();
        m_style->ScaleAllSizes(main_scale);
        m_style->FontScaleDpi = main_scale;

        ImGui_ImplSDL3_InitForOpenGL(m_window, m_glContext);
        ImGui_ImplOpenGL3_Init(m_glslVersion);

        return true;
    }

    void LinuxPlatform::Shutdown()
    {
        if (m_imguiContext)
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext();
            m_imguiContext = nullptr;
            m_io = nullptr;
        }

        if (m_glContext)
        {
            SDL_GL_DestroyContext(m_glContext);
            m_glContext = nullptr;
        }

        if (m_window)
        {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }

        SDL_Quit();
    }

    bool LinuxPlatform::ShouldClose() { return m_shouldClose; }

    void LinuxPlatform::PollEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                m_shouldClose = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(m_window))
                m_shouldClose = true;
        }
    }

    void LinuxPlatform::NewFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    void LinuxPlatform::SetClearColor(ImVec4 &color)
    {
        m_clearColor = color;
    }

    void LinuxPlatform::RenderFrame()
    {
        ImGui::Render();
        glViewport(0, 0, (int)m_io->DisplaySize.x, (int)m_io->DisplaySize.y);
        glClearColor(m_clearColor.x * m_clearColor.w, m_clearColor.y * m_clearColor.w, m_clearColor.z * m_clearColor.w, m_clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(m_window);
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

    void *LinuxPlatform::GetNativeWindow() { return m_window; }

    void *LinuxPlatform::GetNativeRenderer() { return nullptr; }
}