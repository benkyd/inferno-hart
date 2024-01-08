#include "rendertarget.hpp"

#include "buffer.hpp"
#include "device.hpp"
#include "graphics.hpp"
#include "image.hpp"
#include "vkrenderer.hpp"

#include "yolo/yolo.hpp"
#include <vulkan/vulkan_core.h>

namespace inferno::graphics {

RenderTarget* rendertarget_create(
    GraphicsDevice* device, VkExtent2D extent, VkFormat format, bool depth)
{
    RenderTarget* target = new RenderTarget();
    target->Device = device;
    target->Extent = extent;
    target->Format = format;

    VkSamplerCreateInfo samplerInfo {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(device->VulkanDevice, &samplerInfo, nullptr, &target->Sampler)
        != VK_SUCCESS) {
        yolo::error("failed to create texture sampler!");
    }

    target->TargetImage = new ImageAttachment();

    create_image(device, extent.width, extent.height, format, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
            | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, target->TargetImage->Image,
        target->TargetImage->Memory);

    target->TargetImage->ImageView = create_image_view(
        device, target->TargetImage->Image, format, VK_IMAGE_ASPECT_COLOR_BIT);

    target->DescriptorSet = ImGui_ImplVulkan_AddTexture(target->Sampler,
        target->TargetImage->ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // wait for the image to be ready
    graphics::renderer_submit_now(
        device->RenderContext, [&](VulkanRenderer* re, VkCommandBuffer* cmd) {
            transition_image_layout(re->Device, target->TargetImage->Image, format,
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        });

    if (depth) {
        rendertarget_create_depth(target);
    }

    return target;
}

void rendertarget_cleanup(RenderTarget* target)
{
    vkDestroyImageView(
        target->Device->VulkanDevice, target->TargetImage->ImageView, nullptr);
    vkDestroyImage(target->Device->VulkanDevice, target->TargetImage->Image, nullptr);
    vkFreeMemory(target->Device->VulkanDevice, target->TargetImage->Memory, nullptr);

    rendertarget_destroy_depth(target);

    delete target;
}

void rendertarget_create_depth(RenderTarget* target)
{
    VkFormat depthFormat = find_depth_format(target->Device);

    target->TargetDepth = new DepthAttachment();
    target->TargetDepth->Format = depthFormat;

    create_image(target->Device, target->Extent.width, target->Extent.height, depthFormat,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, target->TargetDepth->Image,
        target->TargetDepth->Memory);

    target->TargetDepth->ImageView = create_image_view(target->Device,
        target->TargetDepth->Image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void rendertarget_destroy_depth(RenderTarget* target)
{
    if (target->TargetDepth != nullptr) {
        vkDestroyImageView(
            target->Device->VulkanDevice, target->TargetDepth->ImageView, nullptr);
        vkDestroyImage(target->Device->VulkanDevice, target->TargetDepth->Image, nullptr);
        vkFreeMemory(target->Device->VulkanDevice, target->TargetDepth->Memory, nullptr);
    }
}

void rendertarget_recreate(RenderTarget* target, VkExtent2D extent, VkFormat format)
{
    vkDeviceWaitIdle(target->Device->VulkanDevice);

    bool doDepth = target->TargetDepth != nullptr;

    vkDestroyImageView(
        target->Device->VulkanDevice, target->TargetImage->ImageView, nullptr);
    vkDestroyImage(target->Device->VulkanDevice, target->TargetImage->Image, nullptr);
    vkFreeMemory(target->Device->VulkanDevice, target->TargetImage->Memory, nullptr);

    if (doDepth) {
        vkDestroyImageView(
            target->Device->VulkanDevice, target->TargetDepth->ImageView, nullptr);
        vkDestroyImage(target->Device->VulkanDevice, target->TargetDepth->Image, nullptr);
        vkFreeMemory(target->Device->VulkanDevice, target->TargetDepth->Memory, nullptr);
    }

    target->Extent = extent;
    target->Format = format;

    target->TargetImage = new ImageAttachment();

    create_image(target->Device, extent.width, extent.height, format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
            | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, target->TargetImage->Image,
        target->TargetImage->Memory);

    target->TargetImage->ImageView = create_image_view(
        target->Device, target->TargetImage->Image, format, VK_IMAGE_ASPECT_COLOR_BIT);

    target->DescriptorSet = ImGui_ImplVulkan_AddTexture(target->Sampler,
        target->TargetImage->ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // wait for the image to be ready
    graphics::renderer_submit_now(
        target->Device->RenderContext, [&](VulkanRenderer* re, VkCommandBuffer* cmd) {
            transition_image_layout(re->Device, target->TargetImage->Image, format,
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        });

    if (doDepth) {
        rendertarget_create_depth(target);
    }
}

DynamicCPUTarget* dynamic_rendertarget_create(
    GraphicsDevice* device, VkExtent2D extent, VkFormat format)
{
    DynamicCPUTarget* target = new DynamicCPUTarget();
    target->Format = format;
    target->Extent = extent;
    target->Device = device;

    VkSamplerCreateInfo samplerInfo {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(device->VulkanDevice, &samplerInfo, nullptr, &target->Sampler)
        != VK_SUCCESS) {
        yolo::error("failed to create texture sampler!");
    }

    // Create VkImage with Undefined
    create_image(device, extent.width, extent.height, format, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
            | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, target->Image, target->Memory);

    target->ImageView
        = create_image_view(device, target->Image, format, VK_IMAGE_ASPECT_COLOR_BIT);

    target->DescriptorSet = ImGui_ImplVulkan_AddTexture(
        target->Sampler, target->ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // NOTE: This assumes that the image is in the UNDEFINED LAYOUT
    // And also assumes that the cpu layout is floating point (4 bytes) and the image is
    // RGBA
    target->StagingBuffer = generic_buffer_create(device, 0,
        extent.width * extent.height * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    return target;
}

void dynamic_rendertarget_cleanup(DynamicCPUTarget* target)
{
    vkDestroyImageView(target->Device->VulkanDevice, target->ImageView, nullptr);
    vkDestroyImage(target->Device->VulkanDevice, target->Image, nullptr);
    vkFreeMemory(target->Device->VulkanDevice, target->Memory, nullptr);
    vkDestroySampler(target->Device->VulkanDevice, target->Sampler, nullptr);
    delete target;
}

// TODO: We should do this with a double buffer but whatever
void dynamic_rendertarget_update(DynamicCPUTarget* target, void* data, VkExtent2D size)
{
    if (size.width != target->Extent.width || size.height != target->Extent.height) {
        yolo::error("Dynamic CPU Target size mismatch");
        return;
    }

    // Traansition to General
    transition_image_layout(target->Device, target->Image, target->Format,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    // Sync with Fence
    // we need a fence here because we need to wait for the image to be ready
    // before we can copy to it
    VkFenceCreateInfo fenceInfo {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = 0;

    VkFence fence;
    if (vkCreateFence(target->Device->VulkanDevice, &fenceInfo, nullptr, &fence)
        != VK_SUCCESS) {
        yolo::error("failed to create fence!");
    }

    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.commandBufferCount = 0;
    submitInfo.pCommandBuffers = nullptr;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    if (vkQueueSubmit(target->Device->VulkanGraphicsQueue, 1, &submitInfo, fence)
        != VK_SUCCESS) {
        yolo::error("failed to submit queue!");
    }

    vkWaitForFences(target->Device->VulkanDevice, 1, &fence, VK_TRUE, UINT64_MAX);

    // Map Memory
    void* mappedData;
    vkMapMemory(target->Device->VulkanDevice, target->StagingBuffer->DeviceData, 0,
        target->StagingBuffer->Size, 0, &mappedData);
    memcpy(mappedData, data, target->StagingBuffer->Size);
    vkUnmapMemory(target->Device->VulkanDevice, target->StagingBuffer->DeviceData);

    transition_image_layout(target->Device, target->Image, target->Format,
        VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // No we have the data in the buffer, let's move it to the image
    graphics::renderer_submit_now(
        target->Device->RenderContext, [&](VulkanRenderer* re, VkCommandBuffer* cmd) {
            VkBufferImageCopy region {};
            region.bufferOffset = 0;
            region.bufferRowLength = size.width;
            region.bufferImageHeight = size.height;

            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;

            region.imageOffset = { 0, 0, 0 };
            region.imageExtent = { size.width, size.height, 1 };

            vkCmdCopyBufferToImage(*cmd, target->StagingBuffer->Handle, target->Image,
                VK_IMAGE_LAYOUT_GENERAL, 1, &region);
        });

    // Sync with Fence
    vkResetFences(target->Device->VulkanDevice, 1, &fence);
    vkDestroyFence(target->Device->VulkanDevice, fence, nullptr);

    // Transition to whatever we need
    transition_image_layout(target->Device, target->Image, target->Format,
        VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void dynamic_rendertarget_recreate(DynamicCPUTarget* target, VkExtent2D extent)
{
    vkDestroyImageView(target->Device->VulkanDevice, target->ImageView, nullptr);
    vkDestroyImage(target->Device->VulkanDevice, target->Image, nullptr);
    vkFreeMemory(target->Device->VulkanDevice, target->Memory, nullptr);

    target->Extent = extent;

    // Create VkImage with Undefined
    create_image(target->Device, extent.width, extent.height, target->Format, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
            | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, target->Image, target->Memory);

    target->ImageView
        = create_image_view(target->Device, target->Image, target->Format, VK_IMAGE_ASPECT_COLOR_BIT);

    target->DescriptorSet = ImGui_ImplVulkan_AddTexture(
        target->Sampler, target->ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // NOTE: This assumes that the image is in the UNDEFINED LAYOUT
    // And also assumes that the cpu layout is floating point (4 bytes) and the image is
    // RGBA
    target->StagingBuffer = generic_buffer_create(target->Device, 0,
        extent.width * extent.height * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

}
