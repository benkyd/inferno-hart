#pragma once

#include "graphics.hpp"

namespace inferno::graphics {

#define VALIDATION_LAYERS_ENABLED true
#ifdef VALIDATION_LAYERS_ENABLED
const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation",
};
#endif

typedef struct GraphicsDevice {
    VkInstance VulkanInstance;
    VkDebugUtilsMessengerEXT VulkanDebugMessenger;
    VkPhysicalDevice VulkanPhysicalDevice;
    VkDevice VulkanDevice;
    VkSurfaceKHR VulkanSurface;
    VkQueue VulkanGraphicsQueue;
    VkQueue VulkanPresentQueue;
    VkSwapchainKHR VulkanSwapChain;
} GraphicsDevice;

// MUST ONLY BE CALLED AFTER GLFW INIT
GraphicsDevice* device_create();
void device_cleanup(GraphicsDevice* device);

void device_create_vulkan_instance(GraphicsDevice* device);
void device_vulkan_debugger(GraphicsDevice* device);
void device_create_vulkan_physical_device(GraphicsDevice* device);
void device_create_vulkan_logical_device(GraphicsDevice* device);
}
