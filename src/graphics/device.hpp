#pragma once

#include "graphics.hpp"

#include <optional>

namespace inferno::graphics {

#define VALIDATION_LAYERS_ENABLED 1
#ifdef VALIDATION_LAYERS_ENABLED
const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation",
};
#endif

const std::vector<const char*> DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
};

typedef struct GraphicsDevice {
    VkInstance VulkanInstance;
    VkDebugUtilsMessengerEXT VulkanDebugMessenger;
    VkPhysicalDevice VulkanPhysicalDevice;
    VkDevice VulkanDevice;
    VkSurfaceKHR VulkanSurface;
    VkQueue VulkanGraphicsQueue;
    VkQueue VulkanPresentQueue;
    VkCommandPool VulkanCommandPool;

    glm::ivec2 SurfaceSize;
    bool Resized = false;
} GraphicsDevice;

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

// MUST ONLY BE CALLED AFTER GLFW INIT
GraphicsDevice* device_create();
void device_cleanup(GraphicsDevice* device);

void device_create_vulkan_instance(GraphicsDevice* device);
void device_vulkan_debugger(GraphicsDevice* device);
void device_create_vulkan_physical_device(GraphicsDevice* device);
void device_create_vulkan_logical_device(GraphicsDevice* device);
void device_create_command_pool(GraphicsDevice* device);

void device_resize_callback(GLFWwindow* device, int width, int height);

QueueFamilyIndices device_get_queue_families(GraphicsDevice* g, VkPhysicalDevice device);
uint32_t device_find_memory_type(GraphicsDevice* g, uint32_t typeFilter, VkMemoryPropertyFlags properties);

}
