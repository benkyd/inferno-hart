#pragma once

#include "graphics.hpp"

#include <string>
#include <optional>

#define WINDOW_FLAGS ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse

namespace inferno::graphics {

typedef void (*KeyCallback)(int key, int scan, int action, int mod);
typedef void (*MouseCallback)(double x, double y);

static VkInstance VulkanInstance;
static VkPhysicalDevice VulkanPhysicalDevice = VK_NULL_HANDLE;
static VkDevice VulkanDevice;
static VkQueue VulkanPresentQueue;
static VkSurfaceKHR VulkanSurface;

enum WINDOW_MODE {
    WIN_MODE_DEFAULT,
    WIN_MODE_FPS,
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

void window_create(std::string title, int width, int height);
void window_cleanup();

void window_set_title(std::string title);

void window_set_size(int w, int h);
void window_set_pos(int x, int y);
glm::vec2 window_get_size();
void window_get_pos(int& x, int& y);

// VULKAN SPECIFIC
QueueFamilyIndices window_get_queue_families(VkPhysicalDevice device);
// END VULKAN SPECIFIC

GLFWwindow* window_get_glfw_window();
void window_set_mode(WINDOW_MODE mode);

void window_set_key_callback(KeyCallback callback);
KeyCallback window_get_key_callback();

bool window_new_frame();
void window_render();

}
