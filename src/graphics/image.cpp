#include "graphics/image.hpp"

#include "device.hpp"
#include "vkrenderer.hpp"

#include "yolo/yolo.hpp"

#include <functional>

namespace inferno::graphics {

void create_image(GraphicsDevice* device, uint32_t width, uint32_t height,
    VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
    VkImageCreateInfo imageInfo {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(device->VulkanDevice, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        yolo::error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device->VulkanDevice, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex
        = device_find_memory_type(device, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device->VulkanDevice, &allocInfo, nullptr, &imageMemory)
        != VK_SUCCESS) {
        yolo::error("failed to allocate image memory!");
    }

    vkBindImageMemory(device->VulkanDevice, image, imageMemory, 0);
}

VkImageView create_image_view(GraphicsDevice* device, VkImage image, VkFormat format,
    VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewInfo {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(device->VulkanDevice, &viewInfo, nullptr, &imageView)
        != VK_SUCCESS) {
        yolo::error("failed to create texture image view!");
    }

    return imageView;
}

void transition_image_layout(GraphicsDevice* device, VkImage image, VkFormat format,
    VkImageLayout oldLayout, VkImageLayout newLayout)
{
    // This all goes in a standard callback to go to the renderer as a one time command
    // buffer execution
    std::function<void(VulkanRenderer*, VkCommandBuffer*)> callback
        = [&](VulkanRenderer* renderer, VkCommandBuffer* commandBuffer) {
              VkImageMemoryBarrier barrier {};
              barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
              barrier.oldLayout = oldLayout;
              barrier.newLayout = newLayout;
              barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
              barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
              barrier.image = image;
              barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
              barrier.subresourceRange.baseMipLevel = 0;
              barrier.subresourceRange.levelCount = 1;
              barrier.subresourceRange.baseArrayLayer = 0;
              barrier.subresourceRange.layerCount = 1;
              barrier.srcAccessMask = 0; // TODO
              barrier.dstAccessMask = 0; // TODO

              vkCmdPipelineBarrier(*commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                  VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1,
                  &barrier);
          };

    graphics::renderer_submit_now(device->RenderContext, callback);
}

VkFormat find_format(GraphicsDevice* device, const std::vector<VkFormat>& candidates,
    VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(device->VulkanPhysicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR
            && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL
            && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

VkFormat find_depth_format(GraphicsDevice* device)
{
    return VK_FORMAT_D32_SFLOAT_S8_UINT;
    return find_format(device,
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

}
