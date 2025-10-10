cmake_minimum_required(VERSION 3.20)

project(snap_tools CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Platform detection
if(APPLE)
    set(PLATFORM_NAME "macos")
    set(PLATFORM_BACKEND "metal")
elseif(WIN32)
    set(PLATFORM_NAME "windows") 
    set(PLATFORM_BACKEND "directx11")
elseif(UNIX)
    set(PLATFORM_NAME "unix")
    set(PLATFORM_BACKEND "opengl3")
endif()

message(STATUS "Building for platform: ${PLATFORM_NAME} with ${PLATFORM_BACKEND}")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Dear ImGui base sources
set(IMGUI_DIR ./external/imgui)
set(IMGUI_BASE_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
)

# Platform-specific ImGui backends
if(APPLE)
    list(APPEND IMGUI_BACKEND_SOURCES
        ${IMGUI_DIR}/backends/imgui_impl_metal.mm
        ${IMGUI_DIR}/backends/imgui_impl_osx.mm
    )
elseif(WIN32)
    list(APPEND IMGUI_BACKEND_SOURCES
        ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
        ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
    )
elseif(UNIX)
    list(APPEND IMGUI_BACKEND_SOURCES
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    )
endif()

# Create ImGui library
add_library(imgui STATIC ${IMGUI_BASE_SOURCES} ${IMGUI_BACKEND_SOURCES})
target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# Platform-specific configuration
if(APPLE)
    # macOS: Cocoa + Metal
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(METAL_LIBRARY Metal REQUIRED)
    find_library(METALKIT_LIBRARY MetalKit REQUIRED)
    find_library(QUARTZCORE_LIBRARY QuartzCore REQUIRED)
    find_library(GAMECONTROLLER_LIBRARY GameController REQUIRED)
    
    target_link_libraries(imgui
        ${COCOA_LIBRARY}
        ${METAL_LIBRARY}
        ${METALKIT_LIBRARY}
        ${QUARTZCORE_LIBRARY}
        ${GAMECONTROLLER_LIBRARY}
    )
    
    # Platform sources
    set(PLATFORM_SOURCES
        src/platform/macos/MacOSPlatform.mm
    )
    
elseif(WIN32)
    # Windows: Win32 + DirectX11
    find_package(directx-headers CONFIG REQUIRED)
    
    target_link_libraries(imgui
        d3d11
        dxgi
        d3dcompiler
    )
    
    # Platform sources
    set(PLATFORM_SOURCES
        src/platform/windows/WindowsPlatform.cpp
    )
    
elseif(UNIX)
    # Linux: SDL3 + OpenGL3
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL3 REQUIRED sdl3)
    find_package(OpenGL REQUIRED)
    
    target_include_directories(imgui PUBLIC ${SDL3_INCLUDE_DIRS})
    target_link_libraries(imgui
        ${SDL3_LIBRARIES}
        ${OPENGL_LIBRARIES}
        GL
    )
    
    # Platform sources
    set(PLATFORM_SOURCES
        src/platform/linux/LinuxPlatform.cpp
    )
endif()

# Application sources
set(APP_SOURCES
    main.cpp
    src/Application.cpp
    src/UIManager.cpp
    src/platform/PlatformFactory.cpp
    ${PLATFORM_SOURCES}
)

# Create executable
add_executable(snap_tools ${APP_SOURCES})

# Link libraries
if(APPLE)
    target_link_libraries(snap_tools 
        imgui
        ${COCOA_LIBRARY}
        ${METAL_LIBRARY}
        ${METALKIT_LIBRARY}
        ${QUARTZCORE_LIBRARY}
        ${GAMECONTROLLER_LIBRARY}
    )
    
    # Enable Objective-C++ for .mm files
    set_target_properties(snap_tools PROPERTIES
        OBJCXX_STANDARD 17
        OBJCXX_STANDARD_REQUIRED YES
    )
    
    # Enable ARC for Objective-C++ files to fix memory management
    set_property(SOURCE src/platform/macos/MacOSPlatform.mm PROPERTY COMPILE_FLAGS "-fobjc-arc")
    
elseif(WIN32)
    target_link_libraries(snap_tools 
        imgui
        d3d11
        dxgi
        d3dcompiler
        user32
        gdi32
    )
    
elseif(UNIX)
    target_link_libraries(snap_tools 
        imgui
        ${SDL3_LIBRARIES}
        ${OPENGL_LIBRARIES}
        GL
        pthread
        dl
    )
endif()

# Compiler-specific flags
if(APPLE)
    target_compile_definitions(snap_tools PRIVATE 
        GL_SILENCE_DEPRECATION
    )
endif()

# Debug configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(snap_tools PRIVATE DEBUG=1)
endif()