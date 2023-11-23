#include "renderpass.hpp"

#include "device.hpp"
#include "pipeline.hpp"
#include "swapchain.hpp"

#include "preview_renderer/shader.hpp"

#include "yolo/yolo.hpp"
#include <vulkan/vulkan_core.h>

namespace inferno::graphics {

RenderPass* renderpass_create(GraphicsDevice* device)
{
    RenderPass* renderpass = new RenderPass;
    renderpass->Device = device;
    // TODO: this can be done at the end to reduce the complexity of the pipeline creation
    renderpass->RenderPipeline = pipeline_create(device);

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = renderpass->RenderPipeline->Swap->ImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef; // index = gl_colour in glsl

    VkRenderPassCreateInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(
            device->VulkanDevice, &renderPassInfo, nullptr, &renderpass->VulkanRenderPass)
        != VK_SUCCESS) {
        yolo::error("failed to create render pass!");
        return nullptr;
    }

    Shader* shader = shader_create(device);
    shader_load(shader, "res/shaders/vulkan_test");

    pipeline_configure_to_renderpass(renderpass->RenderPipeline, shader, renderpass);
    return renderpass;
}

void rendepass_cleanup(RenderPass* renderpass)
{
    pipeline_cleanup(renderpass->RenderPipeline);
    vkDestroyRenderPass(
        renderpass->Device->VulkanDevice, renderpass->VulkanRenderPass, nullptr);
    vkDestroyCommandPool(
        renderpass->Device->VulkanDevice, renderpass->CommandPool, nullptr);
    delete renderpass;
}

void renderpass_configure_command_buffer(RenderPass* renderpass)
{
    QueueFamilyIndices i = device_get_queue_families(
        renderpass->Device, renderpass->Device->VulkanPhysicalDevice);
    VkCommandPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = i.graphicsFamily.value();
    poolInfo.flags = 0; // Optional

    if (vkCreateCommandPool(renderpass->Device->VulkanDevice, &poolInfo, nullptr,
            &renderpass->CommandPool)
        != VK_SUCCESS) {
        yolo::error("failed to create command pool!");
    }

    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = renderpass->CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(
            renderpass->Device->VulkanDevice, &allocInfo, &renderpass->CommandBuffer)
        != VK_SUCCESS) {
        yolo::error("failed to allocate command buffers!");
    }

    yolo::debug("Command buffer created");
}

void renderpass_begin(RenderPass* renderpass)
{
    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(renderpass->CommandBuffer, &beginInfo) != VK_SUCCESS) {
        yolo::error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderpass->VulkanRenderPass;
    renderPassInfo.framebuffer
        = renderpass->RenderPipeline->Swap->SwapFramebuffers[renderpass->FrameIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = renderpass->RenderPipeline->Swap->Extent;

    VkClearValue clearColor = { { { 0.0f, 0.3f, 0.3f, 1.0f } } };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(
        renderpass->CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void renderpass_end(RenderPass* renderpass)
{
    vkCmdEndRenderPass(renderpass->CommandBuffer);
    if (vkEndCommandBuffer(renderpass->CommandBuffer) != VK_SUCCESS) {
        yolo::error("failed to record command buffer!");
    }
}

}
