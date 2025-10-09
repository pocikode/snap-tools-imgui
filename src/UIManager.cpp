#include "UIManager.h"
#include <cstdio>

UIManager::UIManager()
    : m_showDemo(true), m_showSettings(false), m_frameTimeBuffer{}, m_frameTimeIndex(0), m_avgFrameTime(16.67f) // 60 FPS initial
{
}

UIManager::~UIManager()
{
    Shutdown();
}

void UIManager::Initialize()
{
    // UI manager initialization (no platform-specific code here)
}

void UIManager::Shutdown()
{
    // UI manager cleanup
}

void UIManager::Update()
{
    // Update frame time statistics efficiently
    UpdateFrameStats();
}

void UIManager::Render()
{
    RenderMainMenuBar();

    if (m_showDemo)
    {
        RenderDemoWindow();
    }

    if (m_showSettings)
    {
        RenderSettingsWindow();
    }
}

void UIManager::RenderMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                // Signal application to close
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Demo Window", nullptr, &m_showDemo);
            ImGui::MenuItem("Settings", nullptr, &m_showSettings);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                // Show about dialog
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void UIManager::UpdateFrameStats()
{
    // Get current frame time from ImGui but cache it to avoid repeated calls
    float currentFrameTime = 1000.0f / ImGui::GetIO().Framerate;

    // Update the circular buffer
    m_frameTimeBuffer[m_frameTimeIndex] = currentFrameTime;
    m_frameTimeIndex = (m_frameTimeIndex + 1) % FRAME_HISTORY_SIZE;

    // Calculate rolling average to smooth out fluctuations
    float sum = 0.0f;
    for (int i = 0; i < FRAME_HISTORY_SIZE; ++i)
    {
        sum += m_frameTimeBuffer[i];
    }
    m_avgFrameTime = sum / FRAME_HISTORY_SIZE;
}

void UIManager::RenderDemoWindow()
{
    // Use local static variables to avoid repeated allocations
    static float f = 0.0f;
    static int counter = 0;
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Pre-allocated buffer for text formatting to avoid string allocations
    static char counter_text[64];
    static char frame_stats[128];

    ImGui::Begin("Snap Tools Demo", &m_showDemo);

    // Use const char* for static text to avoid string operations
    ImGui::TextUnformatted("Cross-Platform ImGui Application");
    ImGui::Separator();

    ImGui::TextUnformatted("This is a cross-platform application using:");
    ImGui::BulletText("macOS: Cocoa + Metal");
    ImGui::BulletText("Windows: Win32 + DirectX11");
    ImGui::BulletText("Linux: SDL3 + OpenGL3");

    ImGui::Separator();

    ImGui::SliderFloat("Float Value", &f, 0.0f, 1.0f);
    ImGui::ColorEdit3("Clear Color", (float *)&clear_color);

    if (ImGui::Button("Click Me!"))
    {
        counter++;
    }
    ImGui::SameLine();

    // Format text into pre-allocated buffer to avoid string allocations
    snprintf(counter_text, sizeof(counter_text), "Counter = %d", counter);
    ImGui::TextUnformatted(counter_text);

    // Use cached frame time instead of calling ImGui::GetIO() repeatedly
    snprintf(frame_stats, sizeof(frame_stats),
             "Application average %.3f ms/frame (%.1f FPS)",
             m_avgFrameTime, 1000.0f / m_avgFrameTime);
    ImGui::TextUnformatted(frame_stats);

    ImGui::End();
}

void UIManager::RenderSettingsWindow()
{
    ImGui::Begin("Settings", &m_showSettings);

    ImGui::Text("Application Settings");
    ImGui::Separator();

    static bool vsync = true;
    ImGui::Checkbox("VSync", &vsync);

    static int samples = 4;
    ImGui::SliderInt("MSAA Samples", &samples, 1, 16);

    if (ImGui::Button("Apply Settings"))
    {
        // Apply settings
    }

    ImGui::End();
}
