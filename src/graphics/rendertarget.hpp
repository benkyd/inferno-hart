#pragma once

#include "graphics.hpp"

#include <optional>

namespace inferno::graphics {

struct GraphicsDevice;

// TODO: Should probably be "ImageAttachment" and then the
// target will abstract which sort of attachment it is
typedef struct DepthAttachment {
    VkImage Image;
    VkDeviceMemory Memory;
    VkImageView ImageView;
    VkFormat Format;
} DepthAttachment;

// TODO: What about the present mode?
typedef struct RenderTarget {
    VkImage Image;
    VkDeviceMemory Memory;
    VkImageView ImageView;

    VkFormat Format;
    VkExtent2D Extent;

    DepthAttachment* TargetDepth = nullptr;

    // NOTE: This is for the ImGui renderer.. it needs a descriptor set of a sampler of an
    // image
    VkSampler Sampler;
    VkDescriptorSet DescriptorSet;

    GraphicsDevice* Device;
} RenderTarget;

RenderTarget* rendertarget_create(
    GraphicsDevice* device, VkExtent2D extent, VkFormat format, bool depth);
void rendertarget_cleanup(RenderTarget* target);

void rendertarget_create_depth(RenderTarget* target);
void rendertarget_destroy_depth(RenderTarget* target);

void rendertarget_recreate(RenderTarget* target, VkExtent2D extent, VkFormat format);

}
