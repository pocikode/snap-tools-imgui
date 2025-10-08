#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

#include "platform/MacOSPlatform.h"
#include "imgui_impl_metal.h"
#include "imgui_impl_osx.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>
@end

@interface ViewController : NSViewController <MTKViewDelegate>
@end

namespace Platform {

class ViewControllerDelegate;

MacOSPlatform::MacOSPlatform() 
    : m_window(nullptr)
    , m_metalView(nullptr)
    , m_metalDevice(nullptr)
    , m_metalCommandQueue(nullptr)
    , m_imguiContext(nullptr)
    , m_shouldClose(false)
    , m_viewController(nullptr) {
}

MacOSPlatform::~MacOSPlatform() {
    Shutdown();
}

bool MacOSPlatform::Initialize(const WindowConfig& config) {
    m_config = config;
    
    @autoreleasepool {
        // Initialize NSApplication
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        
        // Create window
        NSRect frame = NSMakeRect(0, 0, config.width, config.height);
        NSUInteger style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;
        if (config.resizable) {
            style |= NSWindowStyleMaskResizable;
        }
        
        NSWindow* window = [[NSWindow alloc] initWithContentRect:frame
                                              styleMask:style
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
        // With ARC enabled, use bridge cast to transfer ownership
        m_window = (__bridge_retained void*)window;
        
        [window setTitle:[NSString stringWithUTF8String:config.title.c_str()]];
        [window center];
        
        // Initialize Metal
        if (!InitializeRenderer()) {
            return false;
        }
        
        // Initialize ImGui
        if (!InitializeImGui()) {
            return false;
        }
        
        [window makeKeyAndOrderFront:nil];
        [NSApp activateIgnoringOtherApps:YES];
        
        return true;
    }
}

bool MacOSPlatform::InitializeRenderer() {
    @autoreleasepool {
        // Create Metal device
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if (!device) {
            return false;
        }
        m_metalDevice = (__bridge_retained void*)device;
        
        // Create Metal command queue
        id<MTLCommandQueue> commandQueue = [device newCommandQueue];
        m_metalCommandQueue = (__bridge_retained void*)commandQueue;
        
        // Create and retain ViewController
        ViewController* viewController = [[ViewController alloc] init];
        m_viewController = (__bridge_retained void*)viewController;
        
        // Create MTKView
        NSWindow* window = (__bridge NSWindow*)m_window;
        MTKView* metalView = [[MTKView alloc] initWithFrame:[window contentView].bounds device:device];
        
        [metalView setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
        [metalView setDelegate:viewController];
        
        m_metalView = (__bridge_retained void*)metalView;
        
        // Add MTKView to window
        [window setContentView:metalView];
        
        return true;
    }
}

bool MacOSPlatform::InitializeImGui() {
    IMGUI_CHECKVERSION();
    m_imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(m_imguiContext);
    
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    MTKView* metalView = (__bridge MTKView*)m_metalView;
    id<MTLDevice> device = (__bridge id<MTLDevice>)m_metalDevice;
    
    ImGui_ImplOSX_Init(metalView);
    ImGui_ImplMetal_Init(device);
    
    return true;
}

void MacOSPlatform::Shutdown() {
    if (m_imguiContext) {
        ShutdownImGui();
    }
    ShutdownRenderer();
    
    @autoreleasepool {
        if (m_window) {
            NSWindow* window = (__bridge_transfer NSWindow*)m_window;
            [window close];
            m_window = nullptr;
        }
    }
}

void MacOSPlatform::ShutdownRenderer() {
    @autoreleasepool {
        if (m_metalView) {
            MTKView* metalView = (__bridge_transfer MTKView*)m_metalView;
            [metalView setDelegate:nil]; // Remove delegate before releasing
            m_metalView = nullptr;
        }
        if (m_viewController) {
            ViewController* viewController = (__bridge_transfer ViewController*)m_viewController;
            (void)viewController; // ARC will handle the release
            m_viewController = nullptr;
        }
        if (m_metalCommandQueue) {
            id<MTLCommandQueue> commandQueue = (__bridge_transfer id<MTLCommandQueue>)m_metalCommandQueue;
            (void)commandQueue; // ARC will handle the release
            m_metalCommandQueue = nullptr;
        }
        if (m_metalDevice) {
            id<MTLDevice> device = (__bridge_transfer id<MTLDevice>)m_metalDevice;
            (void)device; // ARC will handle the release
            m_metalDevice = nullptr;
        }
    }
}

void MacOSPlatform::ShutdownImGui() {
    ImGui_ImplMetal_Shutdown();
    ImGui_ImplOSX_Shutdown();
    if (m_imguiContext) {
        ImGui::DestroyContext(m_imguiContext);
        m_imguiContext = nullptr;
    }
}

bool MacOSPlatform::ShouldClose() {
    @autoreleasepool {
        NSWindow* window = (__bridge NSWindow*)m_window;
        return m_shouldClose || ![window isVisible];
    }
}

void MacOSPlatform::PollEvents() {
    @autoreleasepool {
        NSEvent* event;
        while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                           untilDate:[NSDate distantPast]
                                              inMode:NSDefaultRunLoopMode
                                             dequeue:YES])) {
            if (event.type == NSEventTypeKeyDown && (event.modifierFlags & NSEventModifierFlagCommand)) {
                if ([event.charactersIgnoringModifiers isEqualToString:@"q"]) {
                    m_shouldClose = true;
                }
            }
            [NSApp sendEvent:event];
        }
    }
}

void MacOSPlatform::SwapBuffers() {
    // MTKView handles this automatically in drawInMTKView
}

void MacOSPlatform::NewFrame() {
    // Metal frame setup
}

void MacOSPlatform::RenderFrame() {
    // Metal rendering
}

void MacOSPlatform::ImGuiNewFrame() {
    @autoreleasepool {
        MTKView* metalView = (__bridge MTKView*)m_metalView;
        MTLRenderPassDescriptor* renderPassDescriptor = [metalView currentRenderPassDescriptor];
        
        ImGui_ImplMetal_NewFrame(renderPassDescriptor);
        ImGui_ImplOSX_NewFrame(metalView);
        ImGui::NewFrame();
    }
}

void MacOSPlatform::ImGuiRender() {
    ImGui::Render();
    
    @autoreleasepool {
        id<MTLCommandQueue> commandQueue = (__bridge id<MTLCommandQueue>)m_metalCommandQueue;
        MTKView* metalView = (__bridge MTKView*)m_metalView;
        
        MTLRenderPassDescriptor* renderPassDescriptor = [metalView currentRenderPassDescriptor];
        if (!renderPassDescriptor) {
            return; // No drawable available
        }
        
        id<CAMetalDrawable> drawable = [metalView currentDrawable];
        if (!drawable) {
            return; // No drawable available
        }
        
        id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
        if (!commandBuffer) {
            return;
        }
        
        // Set a label for debugging
        commandBuffer.label = @"ImGui Render";
        
        id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
        if (!renderEncoder) {
            return;
        }
        
        renderEncoder.label = @"ImGui Encoder";
        
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, renderEncoder);
        [renderEncoder endEncoding];
        
        [commandBuffer presentDrawable:drawable];
        [commandBuffer commit];
        
        // Force completion to prevent command buffer accumulation
        // This helps prevent memory leaks from accumulated command buffers
        [commandBuffer waitUntilCompleted];
    }
}

void MacOSPlatform::ClearBackground(float r, float g, float b, float a) {
    @autoreleasepool {
        MTKView* metalView = (__bridge MTKView*)m_metalView;
        [metalView setClearColor:MTLClearColorMake(r, g, b, a)];
    }
}

void MacOSPlatform::SetWindowTitle(const std::string& title) {
    @autoreleasepool {
        NSWindow* window = (__bridge NSWindow*)m_window;
        [window setTitle:[NSString stringWithUTF8String:title.c_str()]];
    }
}

void MacOSPlatform::GetWindowSize(int& width, int& height) {
    @autoreleasepool {
        NSWindow* window = (__bridge NSWindow*)m_window;
        NSRect frame = [window frame];
        width = (int)frame.size.width;
        height = (int)frame.size.height;
    }
}

void MacOSPlatform::SetWindowSize(int width, int height) {
    @autoreleasepool {
        NSWindow* window = (__bridge NSWindow*)m_window;
        NSRect frame = [window frame];
        frame.size.width = width;
        frame.size.height = height;
        [window setFrame:frame display:YES];
    }
}

void* MacOSPlatform::GetNativeWindow() {
    return m_window;
}

void* MacOSPlatform::GetNativeRenderer() {
    return m_metalDevice;
}

} // namespace Platform

@implementation AppDelegate
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}
@end

@implementation ViewController
- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
    // Handle resize
}

- (void)drawInMTKView:(nonnull MTKView *)view {
    // Drawing is handled by ImGui rendering
}
@end