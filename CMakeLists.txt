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
  set(PLATFORM_NAME "linux")
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
        ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
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
  # macOS: SDL3 + Metal
  # Set PKG_CONFIG_PATH to find Homebrew SDL3
  set(ENV{PKG_CONFIG_PATH} "/opt/homebrew/lib/pkgconfig:/usr/local/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(SDL3 REQUIRED sdl3)
  find_library(METAL_LIBRARY Metal REQUIRED)
  find_library(METALKIT_LIBRARY MetalKit REQUIRED)
  find_library(COCOA_LIBRARY Cocoa REQUIRED)
  find_library(IOKIT_LIBRARY IOKit REQUIRED)
  find_library(CORE_VIDEO_LIBRARY CoreVideo REQUIRED)
  find_library(QUARTZCORE_LIBRARY QuartzCore REQUIRED)

  target_include_directories(imgui PUBLIC ${SDL3_INCLUDE_DIRS})
  target_link_directories(imgui PUBLIC ${SDL3_LIBRARY_DIRS})
  target_link_libraries(imgui
        ${METAL_LIBRARY}
        ${METALKIT_LIBRARY}
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${CORE_VIDEO_LIBRARY}
        ${QUARTZCORE_LIBRARY}
        ${SDL3_LIBRARIES}
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
  target_link_directories(snap_tools PUBLIC ${SDL3_LIBRARY_DIRS})
  target_link_libraries(snap_tools
        imgui
        ${SDL3_LIBRARIES}
        ${COCOA_LIBRARY}
        ${METAL_LIBRARY}
        ${METALKIT_LIBRARY}
        ${QUARTZCORE_LIBRARY}
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
