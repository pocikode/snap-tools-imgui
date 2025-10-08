#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include "platform/IPlatform.h"
#include "UIManager.h"

class Application
{
public:
    Application();
    ~Application();

    bool Initialize();
    void Run();
    void Shutdown();

    bool IsRunning() const { return m_running; }
    void Stop() { m_running = false; }

private:
    std::unique_ptr<Platform::IPlatform> m_platform;
    std::unique_ptr<UIManager> m_ui;
    bool m_running;

    void Update();
    void Render();
};

#endif