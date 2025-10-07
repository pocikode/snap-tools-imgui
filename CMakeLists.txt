cmake_minimum_required(VERSION 4.0)

project(snap_tools CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# PKGBUILD
find_package(PkgConfig REQUIRED)

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW3
pkg_check_modules(GLFW3 REQUIRED glfw3)
include_directories(${CMAKE_SOURCE_DIR}/include ${GLFW3_INCLUDE_DIRS})
link_directories(${GLFW3_LIBRARY_DIRS})

# Dear ImGui
set(IMGUI_DIR ./external/imgui)
set(IMGUI_SOURCES
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
  ${IMGUI_DIR}
  ${IMGUI_DIR}/backends
  ${GLFW3_INCLUDE_DIRS}
)
target_link_libraries(imgui
  glfw
  ${OPENGL_LIBRARIES}
)
target_compile_options(imgui PRIVATE ${GLFW3_CFLAGS_OTHER})

set(APP_SOURCES
  main.cpp
  src/Application.cpp
  src/UIManager.cpp
)

include_directories(include)

add_executable(snap_tools ${APP_SOURCES})
target_link_libraries(snap_tools imgui glfw ${OPENGL_LIBRARIES})
