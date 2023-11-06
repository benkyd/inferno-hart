#pragma once

#include "graphics.hpp"

#include <optional>
#include <string>

#define WINDOW_FLAGS ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse

namespace inferno::graphics {

typedef void (*KeyCallback)(int key, int scan, int action, int mod);
typedef void (*MouseCallback)(double x, double y);

static VkInstance VulkanInstance;
static VkPhysicalDevice VulkanPhysicalDevice = VK_NULL_HANDLE;
static VkDevice VulkanDevice;
static VkQueue VulkanPresentQueue;
static VkSurfaceKHR VulkanSurface;
static VkSwapchainKHR VulkanSwapChain;

enum WINDOW_MODE {
    WIN_MODE_DEFAULT,
    WIN_MODE_FPS,
};

enum HW_CAPABILITY {
    HW_CAPABILITY_VK,
    HW_CAPABILITY_VK_RT,
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

void window_create(std::string title, int width, int height);
void window_cleanup();

void window_set_title(std::string title);

void window_set_size(int w, int h);
void window_set_pos(int x, int y);
glm::vec2 window_get_size();
void window_get_pos(int& x, int& y);

// VULKAN SPECIFIC
bool window_evaluate_device(VkPhysicalDevice device, std::vector<const char*> ext_needed, HW_CAPABILITY* capability);
bool window_evaluate_device_extensions(VkPhysicalDevice device, std::vector<const char*> extensions);
QueueFamilyIndices window_get_queue_families(VkPhysicalDevice device);
SwapChainSupportDetails window_get_swap_chain_support(VkPhysicalDevice device);
// END VULKAN SPECIFIC

GLFWwindow* window_get_glfw_window();
void window_set_mode(WINDOW_MODE mode);

void window_set_key_callback(KeyCallback callback);
KeyCallback window_get_key_callback();

bool window_new_frame();
void window_render();

}
