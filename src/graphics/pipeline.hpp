#pragma once

#include "graphics.hpp"
#include <vulkan/vulkan_core.h>

namespace inferno::graphics {

struct GraphicsDevice;
struct RenderPass;
struct SwapChain;
struct Shader;

typedef struct Pipeline {
    GraphicsDevice* Device;
    SwapChain* Swap;

    VkPipeline GraphicsPipeline;
    VkPipelineLayout Layout;

    VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo;
    VkPipelineVertexInputStateCreateInfo VertexInputInfo;
    VkPipelineInputAssemblyStateCreateInfo InputAssembly;
    VkPipelineViewportStateCreateInfo ViewportState;
    VkPipelineRasterizationStateCreateInfo Rasterizer;
    VkPipelineMultisampleStateCreateInfo Multisampling;
    VkPipelineColorBlendAttachmentState ColorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo ColorBlending;
} Pipeline;

Pipeline* pipeline_create(GraphicsDevice* device);
void pipeline_cleanup(Pipeline* pipeline);

void pipeline_configure_to_renderpass(Pipeline* pipeline, Shader* shader, RenderPass* renderpass);

}
