#ifndef MACOS_PLATFORM_H
#define MACOS_PLATFORM_H

#include "IPlatform.h"
#include "imgui.h"
#include "imgui_impl_metal.h"
#include "imgui_impl_sdl3.h"

// Forward declarations to avoid including Objective-C headers in C++
struct SDL_Window;

#ifdef __OBJC__
@class NSWindow;
@class MTKView;
@protocol MTLDevice;
@protocol MTLCommandQueue;
#else
typedef void NSWindow;
typedef void MTKView;
typedef void *MTLDevice;
typedef void *MTLCommandQueue;
#endif

namespace Platform
{

    class MacOSPlatform : public IPlatform
    {
    public:
        MacOSPlatform();
        ~MacOSPlatform() override;

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
        RendererType GetRendererType() const override { return RendererType::Metal; }

        // ImGui integration
        bool InitializeImGui() override;
        void ShutdownImGui() override;
        void ImGuiNewFrame() override;
        void ImGuiRender() override;

        // Platform-specific getters
        void *GetNativeWindow() override;
        void *GetNativeRenderer() override;

    private:
        SDL_Window *m_window;
        void *m_metalView;
        void *m_metalDevice;
        void *m_metalLayer;
        void *m_renderPassDescriptor;
        void *m_metalCommandQueue;
        void *m_commandBuffer;
        void *m_renderEncoder;
        void *m_viewController;
        void *m_drawable; // Current Metal drawable

        // Dear ImGui
        ImGuiContext *m_imguiContext;
        ImGuiIO *m_io;
        ImGuiStyle *m_style;

        bool m_shouldClose;
        WindowConfig m_config;
    };

} // namespace Platform

#endif // MACOS_PLATFORM_H
