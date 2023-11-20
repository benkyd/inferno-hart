#pragma once

#include "graphics.hpp"

namespace inferno::graphics {

struct GraphicsDevice;
struct Pipeline;

typedef struct RenderPass {
    GraphicsDevice* Device;
    Pipeline* RenderPipeline;

    VkRenderPass RenderPass;
    VkFramebuffer Framebuffer;
} RenderPass;

RenderPass* renderpass_create(GraphicsDevice* device);
void renderpass_cleanup(RenderPass* renderpass);

}
