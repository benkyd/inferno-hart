#pragma once

#include "../graphics.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace inferno::scene {
struct GlobalUniformObject;
}

namespace inferno::graphics {

struct GraphicsDevice;
struct SwapChain;
struct Pipeline;
struct GenBuffer;

typedef enum ShaderProgramType {
    SHADER_PROGRAM_TYPE_GRAPHICS,
    SHADER_PROGRAM_TYPE_GRAPHICS_LINE,
    SHADER_PROGRAM_TYPE_COMPUTE,
    SHADER_PROGRAM_TYPE_RAYTRACING
} ShaderProgramType;

typedef struct ShaderPushConst {
    union {
        glm::mat4 mat[2]; // 64 bytes each
        float data[16];
    };
} ShaderPushConstants;

// TODO: Make general
typedef struct Shader {
    GraphicsDevice* Device;

    ShaderProgramType Type;

    SwapChain* GraphicsSwapchain;
    Pipeline* GraphicsPipeline;

    VkDescriptorPool DescriptorPool;
    VkDescriptorSet Descriptors[FRAMES_IN_FLIGHT];
    VkDescriptorSetLayout DescriptorLayout;

    GenBuffer* GlobalUniformBuffer;

    VkShaderModule VertexShader;
    VkShaderModule FragmentShader;
    VkPipelineShaderStageCreateInfo ShaderStages[SHADER_STAGES];
    VkPipelineVertexInputStateCreateInfo VertexInputInfo;
} Shader;

Shader* shader_create(
    GraphicsDevice* device, SwapChain* swapchain, ShaderProgramType type);
void shader_cleanup(Shader* shader);

void shader_load(Shader* shader, std::filesystem::path path);

void shader_update_state(Shader* shader, VkCommandBuffer commandBuffer,
    scene::GlobalUniformObject object, uint32_t frameIndex);
void shader_push_const(glm::mat4 mat);

// actually creates the pipeline
void shader_build(Shader* shader);

void shader_use(Shader* shader, VkCommandBuffer commandBuffer, VkRect2D renderArea);
void shader_unuse(Shader* shader, VkCommandBuffer commandBuffer);

}
