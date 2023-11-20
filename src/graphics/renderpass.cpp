#include "renderpass.hpp"

#include "device.hpp"
#include "pipeline.hpp"
#include "swapchain.hpp"

#include "preview_renderer/shader.hpp"

#include "yolo/yolo.hpp"

namespace inferno::graphics {

RenderPass* renderpass_create(GraphicsDevice* device)
{
    RenderPass* renderpass = new RenderPass;
    renderpass->Device = device;
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
            device->VulkanDevice, &renderPassInfo, nullptr, &renderpass->RenderPass)
        != VK_SUCCESS) {
        yolo::error("failed to create render pass!");
        return nullptr;
    }

    Shader* shader = shader_create(device->VulkanDevice);
    shader_load(shader, "res/shaders/vulkan_test");

    pipeline_configure_to_renderpass(renderpass->RenderPipeline, shader, renderpass);
    return renderpass;
}

void rendepass_cleanup(RenderPass* renderpass)
{
    pipeline_cleanup(renderpass->RenderPipeline);
    vkDestroyRenderPass(renderpass->Device->VulkanDevice, renderpass->RenderPass, nullptr);
    delete renderpass;
}

}
