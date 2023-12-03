#pragma once

#include "graphics.hpp"

#include <optional>

namespace inferno::graphics {

struct GraphicsDevice;

// TODO: What about the present mode?
typedef struct RenderTarget {
    VkImage Image;
    VkDeviceMemory Memory;
    VkImageView ImageView;

    std::optional<VkImage> DepthImage;
    std::optional<VkDeviceMemory> DepthMemory;
    std::optional<VkImageView> DepthImageView;

    VkFormat Format;
    VkExtent2D Extent;

    GraphicsDevice* Device;
} RenderTarget;

RenderTarget* rendertarget_create(GraphicsDevice* device, VkExtent2D extent, VkFormat format);
void rendertarget_cleanup(RenderTarget* target);

void rendertarget_create_depth(RenderTarget* target);
void rendertarget_destroy_depth(RenderTarget* target);

void rendertarget_recreate(RenderTarget* target, VkExtent2D extent, VkFormat format);

}
