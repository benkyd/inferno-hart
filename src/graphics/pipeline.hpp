#pragma once

#include "graphics.hpp"

namespace inferno::graphics {

struct GraphicsDevice;
struct RenderPass;
struct SwapChain;
struct Shader;

typedef struct Pipeline {
    GraphicsDevice* Device;
    // TODO: Pipeline should not need to know about the swapchain
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
