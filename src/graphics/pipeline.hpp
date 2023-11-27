#pragma once

#include "graphics.hpp"

namespace inferno::graphics {

struct GraphicsDevice;
struct RenderPass;
struct SwapChain;
struct Shader;

typedef struct Pipeline {
    GraphicsDevice* Device;
    SwapChain* Swap;

    VkPipeline GraphicsPipeline;
    VkDescriptorSetLayout DescriptorSetLayout; // WHY
    VkPipelineLayout Layout;

    VkPipelineDynamicStateCreateInfo DynamicStates;
    VkPipelineVertexInputStateCreateInfo VertexInputInfo;
    VkPipelineInputAssemblyStateCreateInfo InputAssembly;
    VkPipelineViewportStateCreateInfo ViewportState;
    VkPipelineRasterizationStateCreateInfo Rasterizer;
    VkPipelineMultisampleStateCreateInfo Multisampling;
    VkPipelineColorBlendAttachmentState ColorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo ColorBlending;
} Pipeline;

Pipeline* pipeline_create(GraphicsDevice* device, SwapChain* swap);
void pipeline_cleanup(Pipeline* pipeline);

void pipeline_create_descriptor_set_layout(Pipeline* pipeline);

}
