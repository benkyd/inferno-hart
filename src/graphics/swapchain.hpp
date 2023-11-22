#pragma once

#include "graphics.hpp"

namespace inferno::graphics {

struct GraphicsDevice;

struct RenderPass;

typedef struct SwapChain {
    VkSwapchainKHR Handle;
    VkFormat ImageFormat;
    VkExtent2D Extent;

    std::vector<VkImage> Images;
    std::vector<VkImageView> ImageViews;

    // Framebuffers
    std::vector<VkFramebuffer> SwapFramebuffers;

    GraphicsDevice* Device;
} SwapChain;

SwapChain* swapchain_create(GraphicsDevice* device, glm::ivec2 surface_size);
void swapchain_cleanup(SwapChain* swapchain);

void swapchain_image_view_create(SwapChain* swapchain);
void swapchain_framebuffers_create(SwapChain* swapchain, RenderPass* renderpass);

}
