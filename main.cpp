#include <stdio.h>
#include <cstdio>
#include <stdexcept>
#include "Application.h"

void app_window_error_callback(int error, const char *description)
{
    printf("Error %d: %s\n", error, description);
}

int main(void)
{
    // initialize GLFW
    glfwSetErrorCallback(app_window_error_callback);
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // create GLFW window
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "KOPET", NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Make sure the window is visible
    glfwShowWindow(window);
    printf("Window created and shown...\n");

    printf("Starting application...\n");
    try
    {
        Application app(window);

        while (!glfwWindowShouldClose(window) && app.running)
        {
            glfwPollEvents();
            app.Render();
            glfwSwapBuffers(window);
        }

        app.Cleanup();
    }
    catch (const std::exception &e)
    {
        fprintf(stderr, "Application error: %s\n", e.what());
    }
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
