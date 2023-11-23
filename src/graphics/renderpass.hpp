#pragma once

#include "graphics.hpp"

namespace inferno::graphics {

struct GraphicsDevice;
struct Pipeline;

typedef struct RenderPass {
    GraphicsDevice* Device;
    Pipeline* RenderPipeline;

    VkRenderPass VulkanRenderPass;

    VkCommandPool CommandPool;
    VkCommandBuffer CommandBuffer;
    int FrameIndex;
} RenderPass;

RenderPass* renderpass_create(GraphicsDevice* device);
void renderpass_cleanup(RenderPass* renderpass);

void renderpass_configure_command_buffer(RenderPass* renderpass);

void renderpass_begin(RenderPass* renderpass);
void renderpass_end(RenderPass* renderpass);

}
