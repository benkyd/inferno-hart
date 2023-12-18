#pragma once

#include "graphics.hpp"

#include <functional>
#include <map>
#include <vector>

namespace inferno::graphics {

struct GraphicsDevice;
struct Pipeline;
struct SwapChain;
struct RenderTarget;
struct GenBuffer;
struct Shader;

// TODO: Make the inflight frames work better
typedef struct FrameInFlight {
    VkSemaphore ImageAvailable;
    VkSemaphore RenderFinished;
    VkFence Fence;

    // Global Uniform Buffer
    // TODO: We also need push constants
    GenBuffer* UniformBuffer;
} FrameInFlight;

typedef struct VulkanRenderer {
    GraphicsDevice* Device;
    SwapChain* Swap;

    // TODO: This is really fucking annoying, how can we
    // not do this? CommandBuffers need to be *sequential*
    // in client memory
    std::vector<VkCommandBuffer> CommandBuffersInFlight;
    VkCommandBuffer* CommandBufferInFlight;

    std::vector<FrameInFlight> InFlight;
    FrameInFlight* CurrentFrame;

    uint32_t CurrentFrameIndex;
    uint32_t ImageIndex;

    std::vector<std::function<void(VulkanRenderer*, VkCommandBuffer*)>>
        SubmitQueueOneOffPreFrame;
    std::vector<std::function<void(VulkanRenderer*, VkCommandBuffer*)>>
        SubmitQueueOneOffPostFrame;
    std::vector<std::function<void(VulkanRenderer*, VkCommandBuffer*)>>
        SubmitQueuePreFrame;
    std::vector<std::function<void(VulkanRenderer*, VkCommandBuffer*)>>
        SubmitQueuePostFrame;
} VulkanRenderer;

VulkanRenderer* renderer_create(GraphicsDevice* device);
void renderer_cleanup(VulkanRenderer* renderer);

void renderer_configure_gui(VulkanRenderer* renderer);
void renderer_configure_command_buffer(VulkanRenderer* renderer);

void renderer_submit_oneoff(VulkanRenderer* renderer,
    std::function<void(VulkanRenderer*, VkCommandBuffer*)> callback, bool post = false);
void renderer_submit_repeat(VulkanRenderer* renderer,
    std::function<void(VulkanRenderer*, VkCommandBuffer*)> callback, bool post = false);
void renderer_submit_now(VulkanRenderer* renderer,
    std::function<void(VulkanRenderer*, VkCommandBuffer*)> callback);

bool renderer_begin_frame(VulkanRenderer* renderer);

void renderer_begin_pass(
    VulkanRenderer* renderer, RenderTarget* target, VkRect2D renderArea);
// this is for rendering to the swapchain / present image
void renderer_begin_pass(VulkanRenderer* renderer, VkRect2D renderArea);

void renderer_end_pass(VulkanRenderer* renderer);

bool renderer_draw_frame(VulkanRenderer* renderer);

}
