#include "../../include/platform/WindowsPlatform.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#ifdef _WIN32

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Platform
{

    WindowsPlatform::WindowsPlatform()
        : m_hwnd(nullptr), m_d3dDevice(nullptr), m_d3dDeviceContext(nullptr), m_swapChain(nullptr), m_mainRenderTargetView(nullptr), m_imguiContext(nullptr), m_shouldClose(false)
    {
    }

    WindowsPlatform::~WindowsPlatform()
    {
        Shutdown();
    }

    bool WindowsPlatform::Initialize(const WindowConfig &config)
    {
        m_config = config;

        // Create window class
        m_wc = {sizeof(m_wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr),
                nullptr, nullptr, nullptr, nullptr, L"SnapToolsApp", nullptr};
        ::RegisterClassExW(&m_wc);

        // Create window
        m_hwnd = ::CreateWindowW(m_wc.lpszClassName, L"Snap Tools - Windows",
                                 WS_OVERLAPPEDWINDOW, 100, 100, config.width, config.height,
                                 nullptr, nullptr, m_wc.hInstance, nullptr);

        if (!m_hwnd)
            return false;

        // Initialize DirectX
        if (!CreateDeviceD3D())
        {
            CleanupDeviceD3D();
            return false;
        }

        // Show window
        ::ShowWindow(m_hwnd, SW_SHOWDEFAULT);
        ::UpdateWindow(m_hwnd);

        return true;
    }

    bool WindowsPlatform::CreateDeviceD3D()
    {
        // Create DirectX 11 device and swap chain
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 2;
        sd.BufferDesc.Width = 0;
        sd.BufferDesc.Height = 0;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = m_hwnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        UINT createDeviceFlags = 0;
        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[2] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_0,
        };

        HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                                    createDeviceFlags, featureLevelArray, 2,
                                                    D3D11_SDK_VERSION, &sd, &m_swapChain,
                                                    &m_d3dDevice, &featureLevel, &m_d3dDeviceContext);

        if (res != S_OK)
            return false;

        CreateRenderTarget();
        return true;
    }

    bool WindowsPlatform::InitializeImGui()
    {
        IMGUI_CHECKVERSION();
        m_imguiContext = ImGui::CreateContext();
        ImGui::SetCurrentContext(m_imguiContext);

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui::StyleColorsDark();

        ImGui_ImplWin32_Init(m_hwnd);
        ImGui_ImplDX11_Init(m_d3dDevice, m_d3dDeviceContext);

        return true;
    }

    void WindowsPlatform::Shutdown()
    {
        ShutdownImGui();
        CleanupDeviceD3D();

        if (m_hwnd)
        {
            ::DestroyWindow(m_hwnd);
            m_hwnd = nullptr;
        }
        ::UnregisterClassW(m_wc.lpszClassName, m_wc.hInstance);
    }

    // ... Additional method implementations would go here ...

    LRESULT WINAPI WindowsPlatform::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg)
        {
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        }
        return ::DefWindowProcW(hWnd, msg, wParam, lParam);
    }

} // namespace Platform

#else
// Stub implementation for non-Windows platforms
namespace Platform
{
    WindowsPlatform::WindowsPlatform() {}
    WindowsPlatform::~WindowsPlatform() {}
    bool WindowsPlatform::Initialize(const WindowConfig &) { return false; }
    void WindowsPlatform::Shutdown() {}
    bool WindowsPlatform::ShouldClose() { return true; }
    void WindowsPlatform::PollEvents() {}
    void WindowsPlatform::SwapBuffers() {}
    void WindowsPlatform::SetWindowTitle(const std::string &) {}
    void WindowsPlatform::GetWindowSize(int &, int &) {}
    void WindowsPlatform::SetWindowSize(int, int) {}
    bool WindowsPlatform::InitializeRenderer() { return false; }
    void WindowsPlatform::ShutdownRenderer() {}
    void WindowsPlatform::NewFrame() {}
    void WindowsPlatform::RenderFrame() {}
    void WindowsPlatform::ClearBackground(float, float, float, float) {}
    bool WindowsPlatform::InitializeImGui() { return false; }
    void WindowsPlatform::ShutdownImGui() {}
    void WindowsPlatform::ImGuiNewFrame() {}
    void WindowsPlatform::ImGuiRender() {}
    void *WindowsPlatform::GetNativeWindow() { return nullptr; }
    void *WindowsPlatform::GetNativeRenderer() { return nullptr; }
}
#endif