#pragma once

#include "graphics.hpp"

namespace inferno::graphics {

struct GraphicsDevice;
struct RenderPass;
struct SwapChain;
struct Shader;

typedef enum PipelineType {
    PIPELINE_TYPE_GRAPHICS,
    PIPELINE_TYPE_GRAPHICS_LINE,
    PIPELINE_TYPE_COMPUTE,
    PIPELINE_TYPE_RAYTRACING
} PipelineType;

typedef struct Pipeline {
    GraphicsDevice* Device;
    SwapChain* Swap;
    Shader* RelaventShader;

    uint32_t DescriptorSetLayoutCount;
    VkDescriptorSetLayout* DescriptorSetLayouts;

    VkPipeline GraphicsPipeline;
    VkPipelineLayout Layout;

    PipelineType Type;
} Pipeline;

Pipeline* pipeline_create(GraphicsDevice* device, SwapChain* swap, Shader* shader,
    uint32_t descriptorSetLayoutCount, VkDescriptorSetLayout* layouts, PipelineType type);
void pipeline_cleanup(Pipeline* pipeline);

void pipeline_recreate(Pipeline* pipeline);

}
