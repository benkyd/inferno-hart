#include "window.hpp"

#include "gui/style.hpp"
#include "graphics/device.hpp"

#include <graphics.hpp>
#include <version.hpp>

#include "yolo/yolo.hpp"

#include <map>
#include <optional>
#include <set>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace inferno::graphics {

static GLFWwindow* Window;
static WINDOW_MODE WinMode = WINDOW_MODE::WIN_MODE_DEFAULT;
static KeyCallback UserKeyCallback = nullptr;
static int Width, Height;

void glfwKeyCallback(GLFWwindow* window, int key, int scancode,
    int action, int mods)
{
    if (UserKeyCallback != nullptr) {
        UserKeyCallback(key, scancode, action, mods);
    }
}

void glfwErrorCallback(int error, const char* description)
{
    yolo::error("[GLFW {}] {}", error, description);
}

void setupWindow(std::string title)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    Window = glfwCreateWindow(Width, Height, title.c_str(), nullptr, nullptr);
}



void shutdownGLFW()
{
    glfwDestroyWindow(Window);
    glfwTerminate();
}

void window_create(std::string title, int width, int height)
{
    Width = width;
    Height = height;
    setupWindow(title);
    glfwSetKeyCallback(Window, glfwKeyCallback);
}

void window_init_device(GraphicsDevice* device, GLFWframebuffersizefun resizeCallback)
{
     if (glfwCreateWindowSurface(device->VulkanInstance, Window, nullptr, &device->VulkanSurface) != VK_SUCCESS) {
         yolo::error("failed to create window surface!");
         exit(1);
    }

    glfwSetWindowUserPointer(Window, device);
    glfwSetFramebufferSizeCallback(Window, (GLFWframebuffersizefun)resizeCallback);
}

void window_cleanup()
{
    shutdownGLFW();
}

void window_set_title(std::string title)
{
    glfwSetWindowTitle(Window, title.c_str());
}

void window_set_size(int w, int h)
{
    Width = w;
    Height = h;
    glfwSetWindowSize(Window, Width, Height);
}

void window_set_pos(int x, int y) { glfwSetWindowPos(Window, x, y); }

glm::vec2 window_get_size() { return { Width, Height }; }

void window_get_pos(int& x, int& y) { glfwGetWindowPos(Window, &x, &y); }

GLFWwindow* window_get_glfw_window() { return Window; }

void window_set_mode(WINDOW_MODE mode)
{
    WinMode = mode;
    if (mode == WINDOW_MODE::WIN_MODE_FPS) {
        glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
}

void window_set_key_callback(KeyCallback callback) { UserKeyCallback = callback; }

KeyCallback window_get_key_callback() { return UserKeyCallback; }

bool window_new_frame()
{
    glfwPollEvents();
    if (WinMode == WIN_MODE_FPS) {
        glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        glfwSetCursorPos(Window, (double)Width / 2, (double)Height / 2);
    }
    if (glfwWindowShouldClose(Window)) {
        return false;
    }

    glfwGetWindowSize(Window, &Width, &Height);

    return true;
}

void window_render()
{
    glfwSwapBuffers(Window);
    // ImGui::UpdatePlatformWindows();
}
}
