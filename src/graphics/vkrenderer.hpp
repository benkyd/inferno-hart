#pragma once

#include "graphics.hpp"

namespace inferno::graphics {

struct GraphicsDevice;
struct Pipeline;
struct SwapChain;

typedef struct VulkanRenderer {
    GraphicsDevice* Device;
    SwapChain* Swap;

    VkSemaphore ImageAvailableSemaphore;
    VkSemaphore RenderFinishedSemaphore;
    VkFence InFlightFence;
} Renderer;

Renderer* renderer_create(GraphicsDevice* device, SwapChain* swapchain);
void renderer_cleanup(Renderer* renderer);

void renderer_begin_frame(Renderer* renderer);
void renderer_draw_frame(Renderer* renderer);

}

