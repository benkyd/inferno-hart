#pragma once

#include "graphics.hpp"

namespace inferno::graphics {

#define FRAMES_IN_FLIGHT 2

struct GraphicsDevice;
struct Pipeline;
struct SwapChain;
struct RenderPass;
struct GenBuffer;

// TODO: Make the inflight frames work better
typedef struct FrameInFlight {
    VkSemaphore ImageAvailable;
    VkSemaphore RenderFinished;
    VkFence Fence;

    GenBuffer* UniformBuffer;
} FrameInFlight;

typedef struct VulkanRenderer {
    GraphicsDevice* Device;
    // FIXME: THIS STRUCTURE SHOULD OWN THE SWAPCHAIN
    // AS EVERY PIPELINE WE CONFIGURE WILL NEED ACCESS
    // IT MAKES NO SENSE FOR THE PIPELINE TO OWN IT
    SwapChain* Swap;

    // TODO: This is really fucking annoying, how can we
    // not do this? CommandBuffers need to be *sequential*
    // in client memory
    std::vector<VkCommandBuffer> CommandBuffersInFlight;
    std::vector<FrameInFlight> InFlight;

    FrameInFlight* CurrentFrame;
    uint32_t CurrentFrameIndex;
    uint32_t ImageIndex;
} Renderer;

Renderer* renderer_create(GraphicsDevice* device, SwapChain* swapchain);
void renderer_cleanup(Renderer* renderer);

void renderer_configure_command_buffer(Renderer* renderpass);

bool renderer_begin_frame(Renderer* renderer, RenderPass* renderpass);
bool renderer_draw_frame(Renderer* renderer, RenderPass* renderpass);

}

