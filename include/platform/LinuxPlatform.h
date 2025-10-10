#ifndef UNIX_PLATFORM_H
#define UNIX_PLATFORM_H

#include "IPlatform.h"
#include "imgui.h"
#include <SDL3/SDL.h>

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
        void SetWindowTitle(const std::string &title) override;
        void GetWindowSize(int &width, int &height) override;
        void SetWindowSize(int width, int height) override;

        // Renderer management
        bool InitializeRenderer() override;
        void NewFrame() override;
        void RenderFrame() override;
        void SetClearColor(ImVec4 &color) override;
        RendererType GetRendererType() const override { return RendererType::OpenGL3; }

        // ImGui integration
        bool InitializeImGui() override;

        // Platform-specific getters
        void *GetNativeWindow() override;
        void *GetNativeRenderer() override;

    private:
        SDL_Window *m_window;
        SDL_GLContext m_glContext;
        WindowConfig m_config;

        ImGuiContext *m_imguiContext;
        ImGuiIO *m_io;
        ImGuiStyle *m_style;
        ImVec4 m_clearColor;
        bool m_shouldClose;
        char *m_glslVersion;
    };

} // namespace Platform

#endif // UNIX_PLATFORM_H