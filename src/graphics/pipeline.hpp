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
    Shader* RelaventShader;

    uint32_t DescriptorSetLayoutCount;
    VkDescriptorSetLayout* DescriptorSetLayouts;

    VkPipeline GraphicsPipeline;
    VkPipelineLayout Layout;

    VkPipelineDynamicStateCreateInfo DynamicStates;
    VkPipelineVertexInputStateCreateInfo VertexInputInfo;
    VkPipelineInputAssemblyStateCreateInfo InputAssembly;
    VkPipelineViewportStateCreateInfo ViewportState;
    VkPipelineRasterizationStateCreateInfo Rasterizer;
    VkPipelineMultisampleStateCreateInfo Multisampling;
    VkPipelineColorBlendAttachmentState ColorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo ColorBlending;
    VkPipelineDepthStencilStateCreateInfo DepthState;
} Pipeline;

Pipeline* pipeline_create(GraphicsDevice* device, SwapChain* swap, Shader* shader,
    uint32_t descriptorSetLayoutCount, VkDescriptorSetLayout* layouts);
void pipeline_cleanup(Pipeline* pipeline);

void pipeline_recreate(Pipeline* pipeline);

}
