#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "imgui.h"

class UIManager
{
public:
    UIManager();
    ~UIManager();

    void Initialize();
    void Shutdown();
    void Update();
    void Render();

private:
    void RenderMainMenuBar();
    void RenderDemoWindow();
    void RenderSettingsWindow();

    // UI state
    bool m_showDemo;
    bool m_showSettings;

    // Performance tracking to avoid string allocations
    static constexpr int FRAME_HISTORY_SIZE = 120;
    float m_frameTimeBuffer[FRAME_HISTORY_SIZE];
    int m_frameTimeIndex;
    float m_avgFrameTime;

    // Cached strings to avoid repeated allocations
    void UpdateFrameStats();
};

#endif // UIMANAGER_H