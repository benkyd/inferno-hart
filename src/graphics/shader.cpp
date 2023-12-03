#include "shader.hpp"

#include "buffer.hpp"
#include "device.hpp"
#include "pipeline.hpp"
#include "swapchain.hpp"

#include "scene/scene.hpp"

#include <fstream>
#include <iostream>

#include "yolo/yolo.hpp"

namespace inferno::graphics {

std::string textFromFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    return std::string(
        (std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
}

Shader* shader_create(GraphicsDevice* device, SwapChain* swapchain)
{
    Shader* shader = new Shader;
    shader->Device = device;
    shader->GraphicsSwapchain = swapchain;

    return shader;
}

void shader_cleanup(Shader* shader)
{
    uniform_buffer_cleanup(shader->GlobalUniformBuffer);
    pipeline_cleanup(shader->GraphicsPipeline);
    vkDestroyDescriptorPool(
        shader->Device->VulkanDevice, shader->DescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(
        shader->Device->VulkanDevice, shader->DescriptorLayout, nullptr);
    vkDestroyShaderModule(shader->Device->VulkanDevice, shader->VertexShader, nullptr);
    vkDestroyShaderModule(shader->Device->VulkanDevice, shader->FragmentShader, nullptr);
    delete shader;
}

void shader_load(Shader* shader, std::filesystem::path path)
{
    std::string shaderPath = path.string();
    std::string vertexShaderPath = shaderPath + ".vert.spv";
    std::string fragmentShaderPath = shaderPath + ".frag.spv";
    assert(std::filesystem::exists(vertexShaderPath));
    assert(std::filesystem::exists(fragmentShaderPath));

    std::string vertexLoadedShaderCode = textFromFile(vertexShaderPath);
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = vertexLoadedShaderCode.size();
    createInfo.pCode = (uint32_t*)(vertexLoadedShaderCode.data());

    if (vkCreateShaderModule(
            shader->Device->VulkanDevice, &createInfo, nullptr, &shader->VertexShader)
        != VK_SUCCESS) {
        yolo::error("failed to create shader module");
    }

    memset(&shader->ShaderStages, 0,
        SHADER_STAGES * sizeof(VkPipelineShaderStageCreateInfo));

    shader->ShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader->ShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader->ShaderStages[0].module = shader->VertexShader;
    shader->ShaderStages[0].pName = "main";
    shader->ShaderStages[0].pSpecializationInfo = nullptr; // Optional
    shader->ShaderStages[0].flags = 0; // Optional
    shader->ShaderStages[0].pNext = nullptr; // Optional

    std::string fragmentLoadedShaderCode = textFromFile(fragmentShaderPath);
    VkShaderModuleCreateInfo createInfo2 = {};
    createInfo2.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo2.codeSize = fragmentLoadedShaderCode.size();
    createInfo2.pCode = (uint32_t*)(fragmentLoadedShaderCode.data());

    if (vkCreateShaderModule(
            shader->Device->VulkanDevice, &createInfo2, nullptr, &shader->FragmentShader)
        != VK_SUCCESS) {
        yolo::error("failed to create shader module");
    }

    shader->ShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader->ShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader->ShaderStages[1].module = shader->FragmentShader;
    shader->ShaderStages[1].pName = "main";
    shader->ShaderStages[0].pSpecializationInfo = nullptr; // Optional
    shader->ShaderStages[0].flags = 0; // Optional
    shader->ShaderStages[0].pNext = nullptr; // Optional
}

void shader_update_state(Shader* shader, VkCommandBuffer commandBuffer,
    scene::GlobalUniformObject object, uint32_t frameIndex)
{
    VkDescriptorSet descriptorSet = shader->Descriptors[frameIndex];

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        shader->GraphicsPipeline->Layout, 0, 1, &descriptorSet, 0, nullptr);

    uniform_buffer_update(shader->GlobalUniformBuffer, &object);

    VkDescriptorBufferInfo bufferInfo {};
    bufferInfo.buffer = shader->GlobalUniformBuffer->Handle;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(scene::GlobalUniformObject);

    VkWriteDescriptorSet descriptorWrite {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(shader->Device->VulkanDevice, 1, &descriptorWrite, 0, nullptr);
}

// build the desriptor set layout and pipeline layout
void shader_build(Shader* shader)
{
    VkDescriptorSetLayoutBinding uboLayoutBinding {};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutCreateInfo layoutInfo {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(
            shader->Device->VulkanDevice, &layoutInfo, nullptr, &shader->DescriptorLayout)
        != VK_SUCCESS) {
        yolo::error("failed to create descriptor set layout!");
    }

    VkDescriptorPoolSize poolSize {};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(
            shader->Device->VulkanDevice, &poolInfo, nullptr, &shader->DescriptorPool)
        != VK_SUCCESS) {
        yolo::error("failed to create descriptor pool!");
    }

    std::vector<VkDescriptorSetLayout> layouts(
        FRAMES_IN_FLIGHT, shader->DescriptorLayout);
    VkDescriptorSetAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = shader->DescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    if (vkAllocateDescriptorSets(
            shader->Device->VulkanDevice, &allocInfo, shader->Descriptors)
        != VK_SUCCESS) {
        yolo::error("failed to allocate descriptor sets!");
    }

    shader->GraphicsPipeline = pipeline_create(shader->Device, shader->GraphicsSwapchain,
        shader, layouts.size(), layouts.data());
    shader->GlobalUniformBuffer
        = uniform_buffer_create<scene::GlobalUniformObject>(shader->Device, true);
}

void shader_use(Shader* shader, VkCommandBuffer commandBuffer, VkRect2D renderArea)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        shader->GraphicsPipeline->GraphicsPipeline);

    VkViewport viewport {};
    viewport.x = 0.0f;
    // viewport.y = static_cast<float>(renderArea.extent.height);
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(renderArea.extent.width);
    viewport.height = static_cast<float>(renderArea.extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor {};
    scissor.offset = { 0, 0 };
    scissor.extent = renderArea.extent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void shader_unuse(Shader* shader, VkCommandBuffer commandBuffer)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VK_NULL_HANDLE);
}

}
