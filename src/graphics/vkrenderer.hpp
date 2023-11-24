#pragma once

#include "graphics.hpp"

namespace inferno::graphics {

struct GraphicsDevice;
struct Pipeline;
struct SwapChain;
struct RenderPass;

typedef struct VulkanRenderer {
    GraphicsDevice* Device;
    // FIXME: THIS STRUCTURE SHOULD OWN THE SWAPCHAIN
    // AS EVERY PIPELINE WE CONFIGURE WILL NEED ACCESS
    // IT MAKES NO SENSE FOR THE PIPELINE TO OWN IT
    SwapChain* Swap;

    VkCommandPool CommandPool;
    VkCommandBuffer CommandBuffer;
    uint32_t FrameIndex;
    uint32_t CurrentFrame;

    VkSemaphore ImageAvailableSemaphore;
    VkSemaphore RenderFinishedSemaphore;
    VkFence InFlightFence;
} Renderer;

Renderer* renderer_create(GraphicsDevice* device, SwapChain* swapchain);
void renderer_cleanup(Renderer* renderer);

void renderer_configure_command_buffer(Renderer* renderpass);

void renderer_begin_frame(Renderer* renderer, RenderPass* renderpass);
void renderer_draw_frame(Renderer* renderer, RenderPass* renderpass);

}

