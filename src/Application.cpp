#include "Application.h"
#include "platform/IPlatform.h"
#include <stdio.h>
#include <iostream>

Application::Application()
    : m_running(false)
{
}

Application::~Application()
{
    Shutdown();
}

bool Application::Initialize()
{
    // Create platform-specific implementation
    m_platform = Platform::CreatePlatform();
    if (!m_platform)
    {
        std::cerr << "Failed to create platform implementation" << std::endl;
        return false;
    }

    // Configure window
    Platform::WindowConfig config;
    config.width = 1200;
    config.height = 800;
    config.title = "Snap Tools - Cross Platform";
    config.resizable = true;
    config.vsync = true;

    // Initialize platform
    if (!m_platform->Initialize(config))
    {
        std::cerr << "Failed to initialize platform" << std::endl;
        return false;
    }

    // Initialize renderer
    if (!m_platform->InitializeRenderer())
    {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }

    // Initialize ImGui
    if (!m_platform->InitializeImGui())
    {
        std::cerr << "Failed to initialize ImGui" << std::endl;
        return false;
    }

    // Create UI manager
    m_ui = std::make_unique<UIManager>();
    m_ui->Initialize();

    m_running = true;

    std::cout << "Application initialized successfully" << std::endl;
    std::cout << "Using renderer: ";
    switch (m_platform->GetRendererType())
    {
    case Platform::RendererType::OpenGL3:
        std::cout << "OpenGL 3" << std::endl;
        break;
    case Platform::RendererType::DirectX11:
        std::cout << "DirectX 11" << std::endl;
        break;
    case Platform::RendererType::Metal:
        std::cout << "Metal" << std::endl;
        break;
    }

    return true;
}

void Application::Run()
{
    while (m_running && !m_platform->ShouldClose())
    {
        m_platform->PollEvents();
        Update();
        Render();
        m_platform->SwapBuffers();
    }
}

void Application::Update()
{
    // Update application logic here
    if (m_ui)
    {
        m_ui->Update();
    }
}

void Application::Render()
{
    // Clear background
    m_platform->ClearBackground(0.45f, 0.55f, 0.60f, 1.0f);

    // Start new frame
    m_platform->NewFrame();
    m_platform->ImGuiNewFrame();

    // Render UI
    if (m_ui)
    {
        m_ui->Render();
    }

    // Render ImGui
    m_platform->ImGuiRender();
    m_platform->RenderFrame();
}

void Application::Shutdown()
{
    if (m_ui)
    {
        m_ui->Shutdown();
        m_ui.reset();
    }

    if (m_platform)
    {
        m_platform->Shutdown();
        m_platform.reset();
    }

    m_running = false;
    std::cout << "Application shutdown complete" << std::endl;
}