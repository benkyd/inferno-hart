#include "rendertarget.hpp"

#include "device.hpp"
#include "graphics.hpp"
#include "image.hpp"

#include "yolo/yolo.hpp"

namespace inferno::graphics {

RenderTarget* rendertarget_create(
    GraphicsDevice* device, VkExtent2D extent, VkFormat format, bool depth)
{
    RenderTarget* target = new RenderTarget();
    target->Device = device;
    target->Format = format;
    target->Extent = extent;

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

    create_image(device, extent.width, extent.height, format, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, target->Image, target->Memory);

    target->ImageView
        = create_image_view(device, target->Image, format, VK_IMAGE_ASPECT_COLOR_BIT);

    target->DescriptorSet = ImGui_ImplVulkan_AddTexture(
        target->Sampler, target->ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    return target;
}

void rendertarget_cleanup(RenderTarget* target)
{
    vkDestroyImageView(target->Device->VulkanDevice, target->ImageView, nullptr);
    vkDestroyImage(target->Device->VulkanDevice, target->Image, nullptr);
    vkFreeMemory(target->Device->VulkanDevice, target->Memory, nullptr);

    rendertarget_destroy_depth(target);

    delete target;
}

void rendertarget_create_depth(RenderTarget* target)
{
    VkFormat depthFormat = find_depth_format(target->Device);
    target->DepthFormat = depthFormat;

    create_image(target->Device, target->Extent.width, target->Extent.height, depthFormat,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, target->DepthImage.value(),
        target->DepthMemory.value());

    target->DepthImageView = create_image_view(target->Device, target->DepthImage.value(),
        depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void rendertarget_destroy_depth(RenderTarget* target)
{
    if (target->DepthImageView.has_value()) {
        vkDestroyImageView(
            target->Device->VulkanDevice, target->DepthImageView.value(), nullptr);
    }
    if (target->DepthImage.has_value()) {
        vkDestroyImage(target->Device->VulkanDevice, target->DepthImage.value(), nullptr);
    }
    if (target->DepthMemory.has_value()) {
        vkFreeMemory(target->Device->VulkanDevice, target->DepthMemory.value(), nullptr);
    }
}

void rendertarget_recreate(RenderTarget* target, VkExtent2D extent, VkFormat format)
{
    vkDeviceWaitIdle(target->Device->VulkanDevice);

    bool doDepth = target->DepthImage.has_value();

    rendertarget_cleanup(target);

    target->Extent = extent;
    target->Format = format;

    if (doDepth)
        rendertarget_create_depth(target);

    create_image(target->Device, extent.width, extent.height, format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, target->Image, target->Memory);

    target->ImageView = create_image_view(
        target->Device, target->Image, format, VK_IMAGE_ASPECT_COLOR_BIT);
}

}
