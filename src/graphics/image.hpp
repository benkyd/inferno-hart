#pragma once

#include "graphics.hpp"

namespace inferno::graphics {

struct GraphicsDevice;

void create_image(GraphicsDevice* device, uint32_t width, uint32_t height,
    VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

VkImageView create_image_view(GraphicsDevice* device, VkImage image, VkFormat format,
    VkImageAspectFlags aspectFlags);

void transition_image_layout(GraphicsDevice* device, VkImage image, VkFormat format,
    VkImageLayout oldLayout, VkImageLayout newLayout);
// void copy_buffer_to_image(GraphicsDevice* device, VkBuffer buffer, VkImage image,
//     uint32_t width, uint32_t height);

VkFormat find_format(GraphicsDevice* device, const std::vector<VkFormat>& candidates,
    VkImageTiling tiling, VkFormatFeatureFlags features);
VkFormat find_depth_format(GraphicsDevice* device);

}
