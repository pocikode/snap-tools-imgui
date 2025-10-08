#ifndef IPLATFORM_H
#define IPLATFORM_H

#include <string>
#include <memory>

// Forward declarations
struct ImGuiContext;
struct ImGuiIO;

namespace Platform
{

    enum class RendererType
    {
        OpenGL3,
        DirectX11,
        Metal
    };

    struct WindowConfig
    {
        int width = 1200;
        int height = 800;
        std::string title = "Snap Tools";
        bool resizable = true;
        bool vsync = true;
    };

    class IPlatform
    {
    public:
        virtual ~IPlatform() = default;

        // Window management
        virtual bool Initialize(const WindowConfig &config) = 0;
        virtual void Shutdown() = 0;
        virtual bool ShouldClose() = 0;
        virtual void PollEvents() = 0;
        virtual void SwapBuffers() = 0;
        virtual void SetWindowTitle(const std::string &title) = 0;
        virtual void GetWindowSize(int &width, int &height) = 0;
        virtual void SetWindowSize(int width, int height) = 0;

        // Renderer management
        virtual bool InitializeRenderer() = 0;
        virtual void ShutdownRenderer() = 0;
        virtual void NewFrame() = 0;
        virtual void RenderFrame() = 0;
        virtual void ClearBackground(float r, float g, float b, float a) = 0;
        virtual RendererType GetRendererType() const = 0;

        // ImGui integration
        virtual bool InitializeImGui() = 0;
        virtual void ShutdownImGui() = 0;
        virtual void ImGuiNewFrame() = 0;
        virtual void ImGuiRender() = 0;

        // Platform-specific getters
        virtual void *GetNativeWindow() = 0;
        virtual void *GetNativeRenderer() = 0;
    };

    // Factory function
    std::unique_ptr<IPlatform> CreatePlatform();

} // namespace Platform

#endif // IPLATFORM_H