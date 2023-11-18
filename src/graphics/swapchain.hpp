#pragma once

#include "graphics.hpp"

namespace inferno::graphics {

struct GraphicsDevice;

typedef struct SwapChain {
    VkSwapchainKHR Handle;
    VkFormat ImageFormat;
    VkExtent2D Extent;

    std::vector<VkImage> Images;

    GraphicsDevice* Device;
} SwapChain;

SwapChain* swapchain_create(GraphicsDevice* device, glm::ivec2 surface_size);
void swapchain_cleanup(SwapChain* swapchain);

}
