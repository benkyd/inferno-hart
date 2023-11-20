#pragma once

#include "../graphics.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace inferno::graphics {

// TODO: Make general
typedef struct Shader {
    VkDevice Device;
    VkShaderModule VertexShader;
    VkShaderModule FragmentShader;
    VkPipelineShaderStageCreateInfo ShaderStages[2];
    VkPipelineVertexInputStateCreateInfo VertexInputInfo;
} Shader;

Shader* shader_create(VkDevice device);
void shader_cleanup(Shader* shader);

void shader_load(Shader* shader, std::filesystem::path path);

// TODO: Implement shader_reload
// although with vulkan, it's not really necessary
// as the pipeline is created with the shaders

void shader_use(Shader* shader);
void shader_unuse(Shader* shader);

}
