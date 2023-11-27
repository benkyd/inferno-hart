#pragma once

#include "graphics.hpp"

namespace inferno::graphics {

#define FRAMES_IN_FLIGHT 2

struct GraphicsDevice;
struct Pipeline;
struct SwapChain;
struct RenderPass;

// TODO: Make the inflight frames work better
typedef struct FrameInFlight {
    VkSemaphore ImageAvailable;
    VkSemaphore RenderFinished;
    VkFence InFlight;
} FrameInFlight;

typedef struct VulkanRenderer {
    GraphicsDevice* Device;
    // FIXME: THIS STRUCTURE SHOULD OWN THE SWAPCHAIN
    // AS EVERY PIPELINE WE CONFIGURE WILL NEED ACCESS
    // IT MAKES NO SENSE FOR THE PIPELINE TO OWN IT
    SwapChain* Swap;

    std::vector<VkCommandBuffer> CommandBuffers;
    uint32_t CurrentFrame;
    uint32_t ImageIndex;

    std::vector<VkSemaphore> ImageAvailableSemaphores;
    std::vector<VkSemaphore> RenderFinishedSemaphores;
    std::vector<VkFence> InFlightFences;
} Renderer;

Renderer* renderer_create(GraphicsDevice* device, SwapChain* swapchain);
void renderer_cleanup(Renderer* renderer);

void renderer_configure_command_buffer(Renderer* renderpass);

bool renderer_begin_frame(Renderer* renderer, RenderPass* renderpass);
bool renderer_draw_frame(Renderer* renderer, RenderPass* renderpass);

}

