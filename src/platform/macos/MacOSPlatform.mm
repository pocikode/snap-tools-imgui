#include "imgui_impl_sdl3.h"
#include "platform/MacOSPlatform.h"
#include <Metal/Metal.h>
#include <QuartzCore/QuartzCore.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <cstddef>
#include <iostream>

namespace Platform
{
    MacOSPlatform::MacOSPlatform() : m_window(nullptr),
                                     m_metalLayer(nullptr),
                                     m_renderPassDescriptor(nullptr),
                                     m_imguiContext(nullptr),
                                     m_io(nullptr),
                                     m_style(nullptr),
                                     m_drawable(nullptr),
                                     m_shouldClose(false) {}

    MacOSPlatform::~MacOSPlatform() { Shutdown(); }

    bool MacOSPlatform::Initialize(const WindowConfig &config)
    {
        // Initialize SDL
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
        {
            std::cout << "Failed to init SDL: " << SDL_GetError() << std::endl;
            return false;
        }

        // Create window
        float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
        SDL_WindowFlags window_flags = SDL_WINDOW_METAL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
        m_window = SDL_CreateWindow(config.title.c_str(), (int)(config.width * main_scale), (int)(config.height * main_scale), window_flags);
        if (m_window == nullptr)
        {
            std::cout << "Failed to create SDL window: " << SDL_GetError() << std::endl;
            return false;
        }

        SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(m_window);
        return true;
    }

    bool MacOSPlatform::InitializeRenderer()
    {
        std::cout << "Initializing Metal renderer..." << std::endl;
        
        id<MTLDevice> metalDevice = MTLCreateSystemDefaultDevice();
        if (!metalDevice)
        {
            std::cout << "Failed to create Metal device." << std::endl;
            return false;
        }
        std::cout << "Metal device created successfully" << std::endl;

        SDL_MetalView view = SDL_Metal_CreateView(m_window);
        if (!view) {
            std::cout << "Failed to create SDL Metal view" << std::endl;
            return false;
        }
        std::cout << "SDL Metal view created successfully" << std::endl;
        
        CAMetalLayer *metalLayer = (__bridge CAMetalLayer *)SDL_Metal_GetLayer(view);
        if (!metalLayer) {
            std::cout << "Failed to get Metal layer from view" << std::endl;
            return false;
        }
        m_metalLayer = (__bridge_retained void *)metalLayer;
        metalLayer.device = metalDevice;
        metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        std::cout << "Metal layer configured successfully" << std::endl;

        id<MTLCommandQueue> commandQueue = [metalDevice newCommandQueue];
        if (!commandQueue) {
            std::cout << "Failed to create Metal command queue" << std::endl;
            return false;
        }
        m_metalCommandQueue = (__bridge_retained void *)commandQueue;
        std::cout << "Metal command queue created successfully" << std::endl;
        
        MTLRenderPassDescriptor *renderPass = [MTLRenderPassDescriptor new];
        if (!renderPass) {
            std::cout << "Failed to create render pass descriptor" << std::endl;
            return false;
        }
        m_renderPassDescriptor = (__bridge_retained void *)renderPass;
        std::cout << "Render pass descriptor created successfully" << std::endl;

        return true;
    }

    bool MacOSPlatform::InitializeImGui()
    {
        IMGUI_CHECKVERSION();
        m_imguiContext = ImGui::CreateContext();
        m_io = &ImGui::GetIO();
        m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        float scale = SDL_GetWindowDisplayScale(m_window);
        ImGui::StyleColorsDark();
        m_style = &ImGui::GetStyle();
        m_style->ScaleAllSizes(scale);
        m_style->FontScaleDpi = scale;

        CAMetalLayer *metalLayer = (__bridge CAMetalLayer *)m_metalLayer;
        ImGui_ImplMetal_Init(metalLayer.device);
        ImGui_ImplSDL3_InitForMetal(m_window);

        return true;
    }

    void MacOSPlatform::Shutdown()
    {
        ShutdownImGui();
        ShutdownRenderer();

        if (m_window)
        {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }

        SDL_Quit();
    }

    void MacOSPlatform::ShutdownRenderer()
    {
        // Clean up Metal resources - release retained objects
        if (m_metalLayer) {
            CFRelease(m_metalLayer);
            m_metalLayer = nullptr;
        }
        if (m_renderPassDescriptor) {
            CFRelease(m_renderPassDescriptor);
            m_renderPassDescriptor = nullptr;
        }
        if (m_metalCommandQueue) {
            CFRelease(m_metalCommandQueue);
            m_metalCommandQueue = nullptr;
        }
        
        m_commandBuffer = nullptr;
        m_renderEncoder = nullptr;
        m_drawable = nullptr;
    }

    void MacOSPlatform::ShutdownImGui()
    {
        if (m_imguiContext)
        {
            ImGui_ImplMetal_Shutdown();
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext(m_imguiContext);
            m_imguiContext = nullptr;
            m_io = nullptr;
            m_style = nullptr;
        }
    }

    bool MacOSPlatform::ShouldClose()
    {
        return m_shouldClose;
    }

    void MacOSPlatform::PollEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                m_shouldClose = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(m_window))
                m_shouldClose = true;
        }
    }

    void MacOSPlatform::SwapBuffers()
    {
        // For Metal, buffer swapping is handled by presentDrawable in ImGuiRender
        // This method is kept for interface compatibility
    }

    void MacOSPlatform::NewFrame() {}

    void MacOSPlatform::RenderFrame() {}

    void MacOSPlatform::ImGuiNewFrame()
    {
        MTLRenderPassDescriptor *renderPass = (__bridge MTLRenderPassDescriptor *)m_renderPassDescriptor;
        ImGui_ImplMetal_NewFrame(renderPass);
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    void MacOSPlatform::ImGuiRender()
    {
        ImGui::Render();
        ImDrawData *draw_data = ImGui::GetDrawData();
        
        id<MTLCommandBuffer> commandBuffer = (__bridge id<MTLCommandBuffer>)m_commandBuffer;
        id<MTLRenderCommandEncoder> renderEncoder = (__bridge id<MTLRenderCommandEncoder>)m_renderEncoder;
        
        if (!commandBuffer) {
            std::cout << "Error: Command buffer is null in ImGuiRender" << std::endl;
            return;
        }
        
        if (!renderEncoder) {
            std::cout << "Error: Render encoder is null in ImGuiRender" << std::endl;
            return;
        }
        
        ImGui_ImplMetal_RenderDrawData(draw_data, commandBuffer, renderEncoder);

        [renderEncoder popDebugGroup];
        [renderEncoder endEncoding];

        id<CAMetalDrawable> drawable = (__bridge id<CAMetalDrawable>)m_drawable;
        if (!drawable) {
            std::cout << "Error: Drawable is null in ImGuiRender" << std::endl;
            return;
        }
        
        [commandBuffer presentDrawable:drawable];
        [commandBuffer commit];
    }

    void MacOSPlatform::ClearBackground(float r, float g, float b, float a)
    {
        std::cout << "ClearBackground: Starting..." << std::endl;
        
        int width, height;
        SDL_GetWindowSize(m_window, &width, &height);
        std::cout << "ClearBackground: Window size: " << width << "x" << height << std::endl;

        CAMetalLayer *metalLayer = (__bridge CAMetalLayer *)m_metalLayer;
        if (!metalLayer) {
            std::cout << "Error: Metal layer is null in ClearBackground" << std::endl;
            return;
        }
        std::cout << "ClearBackground: Metal layer OK" << std::endl;
        
        metalLayer.drawableSize = CGSizeMake(width, height);
        std::cout << "ClearBackground: Set drawable size" << std::endl;
        
        // Try to get drawable with retry logic
        id<CAMetalDrawable> drawable = nil;
        for (int i = 0; i < 3; i++) {
            drawable = [metalLayer nextDrawable];
            if (drawable) break;
            std::cout << "ClearBackground: Retry getting drawable, attempt " << (i + 1) << std::endl;
        }
        
        if (!drawable) {
            std::cout << "Error: Failed to get drawable in ClearBackground after retries" << std::endl;
            return;
        }
        std::cout << "ClearBackground: Got drawable successfully" << std::endl;
        m_drawable = (__bridge void *)drawable;

        id<MTLCommandQueue> commandQueue = (__bridge id<MTLCommandQueue>)m_metalCommandQueue;
        if (!commandQueue) {
            std::cout << "Error: Command queue is null in ClearBackground" << std::endl;
            return;
        }
        std::cout << "ClearBackground: Command queue OK" << std::endl;
        
        id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
        if (!commandBuffer) {
            std::cout << "Error: Failed to create command buffer in ClearBackground" << std::endl;
            return;
        }
        std::cout << "ClearBackground: Command buffer created" << std::endl;
        m_commandBuffer = (__bridge void *)commandBuffer;
        
        MTLRenderPassDescriptor *renderPass = (__bridge MTLRenderPassDescriptor *)m_renderPassDescriptor;
        if (!renderPass) {
            std::cout << "Error: Render pass descriptor is null in ClearBackground" << std::endl;
            return;
        }
        std::cout << "ClearBackground: Render pass descriptor OK" << std::endl;
        
        if (!drawable.texture) {
            std::cout << "Error: Drawable texture is null in ClearBackground" << std::endl;
            return;
        }
        std::cout << "ClearBackground: Drawable texture OK" << std::endl;
        
        renderPass.colorAttachments[0].clearColor = MTLClearColorMake(r, g, b, a);
        renderPass.colorAttachments[0].texture = drawable.texture;
        renderPass.colorAttachments[0].loadAction = MTLLoadActionClear;
        renderPass.colorAttachments[0].storeAction = MTLStoreActionStore;
        std::cout << "ClearBackground: Render pass configured" << std::endl;
        
        id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPass];
        if (!renderEncoder) {
            std::cout << "Error: Failed to create render encoder in ClearBackground" << std::endl;
            return;
        }
        std::cout << "ClearBackground: Render encoder created" << std::endl;
        m_renderEncoder = (__bridge void *)renderEncoder;
        [renderEncoder pushDebugGroup:@"SNAP ImGui"];
        std::cout << "ClearBackground: Completed successfully" << std::endl;
    }

    void MacOSPlatform::SetWindowTitle(const std::string &title)
    {
        SDL_SetWindowTitle(m_window, title.c_str());
    }

    void MacOSPlatform::GetWindowSize(int &width, int &height)
    {
        SDL_GetWindowSize(m_window, &width, &height);
    }

    void MacOSPlatform::SetWindowSize(int width, int height)
    {
        SDL_SetWindowSize(m_window, width, height);
    }

    void *MacOSPlatform::GetNativeWindow()
    {
        return m_window;
    }

    void *MacOSPlatform::GetNativeRenderer()
    {
        CAMetalLayer *metalLayer = (__bridge CAMetalLayer *)m_metalLayer;
        return (__bridge void *)metalLayer.device;
    }
}
