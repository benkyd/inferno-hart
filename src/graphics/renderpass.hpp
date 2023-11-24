#pragma once

#include "graphics.hpp"

namespace inferno::graphics {

struct GraphicsDevice;
struct Pipeline;

typedef struct RenderPass {
    GraphicsDevice* Device;
    Pipeline* RenderPipeline;

    VkRenderPass VulkanRenderPass;
} RenderPass;

RenderPass* renderpass_create(GraphicsDevice* device);
void renderpass_cleanup(RenderPass* renderpass);

void renderpass_begin(RenderPass* renderpass, uint32_t imageIndex);
void renderpass_end(RenderPass* renderpass);

}
