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
    SwapChain* Swap;

    // NOTE: TEMPORARY
    Pipeline* RenderPipeline;

    // TODO: This is really fucking annoying, how can we
    // not do this? CommandBuffers need to be *sequential*
    // in client memory
    std::vector<VkCommandBuffer> CommandBuffersInFlight;
    std::vector<FrameInFlight> InFlight;

    FrameInFlight* CurrentFrame;
    uint32_t CurrentFrameIndex;
    uint32_t ImageIndex;
} Renderer;

Renderer* renderer_create(GraphicsDevice* device);
void renderer_cleanup(Renderer* renderer);

void renderer_configure_command_buffer(Renderer* renderer);

bool renderer_begin_frame(Renderer* renderer);
bool renderer_draw_frame(Renderer* renderer);

}

