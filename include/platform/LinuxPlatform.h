#ifndef LINUX_PLATFORM_H
#define LINUX_PLATFORM_H

#include "IPlatform.h"
#include "imgui.h"

#ifdef __linux__
#include <SDL3/SDL.h>
#include <GL/gl3w.h>
#endif

namespace Platform
{

    class LinuxPlatform : public IPlatform
    {
    public:
        LinuxPlatform();
        ~LinuxPlatform() override;

        // Window management
        bool Initialize(const WindowConfig &config) override;
        void Shutdown() override;
        bool ShouldClose() override;
        void PollEvents() override;
        void SwapBuffers() override;
        void SetWindowTitle(const std::string &title) override;
        void GetWindowSize(int &width, int &height) override;
        void SetWindowSize(int width, int height) override;

        // Renderer management
        bool InitializeRenderer() override;
        void ShutdownRenderer() override;
        void NewFrame() override;
        void RenderFrame() override;
        void ClearBackground(float r, float g, float b, float a) override;
        RendererType GetRendererType() const override { return RendererType::OpenGL3; }

        // ImGui integration
        bool InitializeImGui() override;
        void ShutdownImGui() override;
        void ImGuiNewFrame() override;
        void ImGuiRender() override;

        // Platform-specific getters
        void *GetNativeWindow() override;
        void *GetNativeRenderer() override;

    private:
#ifdef __linux__
        SDL_Window *m_window;
        SDL_GLContext m_glContext;
#else
        void *m_window;
        void *m_glContext;
#endif

        ImGuiContext *m_imguiContext;
        bool m_shouldClose;
        WindowConfig m_config;
    };

} // namespace Platform

#endif // LINUX_PLATFORM_H