#ifndef WINDOWS_PLATFORM_H
#define WINDOWS_PLATFORM_H

#include "IPlatform.h"
#include "imgui.h"

#ifdef _WIN32
#include <windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#endif

namespace Platform
{

    class WindowsPlatform : public IPlatform
    {
    public:
        WindowsPlatform();
        ~WindowsPlatform() override;

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
        RendererType GetRendererType() const override { return RendererType::DirectX11; }

        // ImGui integration
        bool InitializeImGui() override;
        void ShutdownImGui() override;
        void ImGuiNewFrame() override;
        void ImGuiRender() override;

        // Platform-specific getters
        void *GetNativeWindow() override;
        void *GetNativeRenderer() override;

    private:
#ifdef _WIN32
        HWND m_hwnd;
        WNDCLASSEXW m_wc;
        ID3D11Device *m_d3dDevice;
        ID3D11DeviceContext *m_d3dDeviceContext;
        IDXGISwapChain *m_swapChain;
        ID3D11RenderTargetView *m_mainRenderTargetView;
#else
        void *m_hwnd;
        void *m_wc;
        void *m_d3dDevice;
        void *m_d3dDeviceContext;
        void *m_swapChain;
        void *m_mainRenderTargetView;
#endif

        ImGuiContext *m_imguiContext;
        bool m_shouldClose;
        WindowConfig m_config;

#ifdef _WIN32
        static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        bool CreateDeviceD3D();
        void CleanupDeviceD3D();
        void CreateRenderTarget();
        void CleanupRenderTarget();
#endif
    };

} // namespace Platform

#endif // WINDOWS_PLATFORM_H