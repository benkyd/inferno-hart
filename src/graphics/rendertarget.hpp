#pragma once

#include "graphics.hpp"

#include <optional>
#include <vulkan/vulkan_core.h>

namespace inferno::graphics {

struct GraphicsDevice;
struct GenBuffer;

// TODO: Should probably be "ImageAttachment" and then the
// target will abstract which sort of attachment it is
typedef struct ImageAttachment {
    VkImage Image;
    VkDeviceMemory Memory;
    VkImageView ImageView;
    VkFormat Format;
} DepthAttachment;

// TODO: What about the present mode?
typedef struct RenderTarget {
    ImageAttachment* TargetImage = nullptr;
    ImageAttachment* TargetDepth = nullptr;

    VkExtent2D Extent;
    VkFormat Format;

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

typedef struct DynamicCPUTarget {
    VkImage Image;
    VkDeviceMemory Memory;
    VkImageView ImageView;

    VkFormat Format;
    VkExtent2D Extent;

    GenBuffer* StagingBuffer;

    // This is being given to ImGUI so we need a Sampler and a DescriptorSet
    VkSampler Sampler;
    VkDescriptorSet DescriptorSet;

    GraphicsDevice* Device;
} DynamicCPUTarget;

DynamicCPUTarget* dynamic_rendertarget_create(
    GraphicsDevice* device, VkExtent2D extent, VkFormat format);
void dynamic_rendertarget_cleanup(DynamicCPUTarget* target);

void dynamic_rendertarget_update(DynamicCPUTarget* target, void* data, VkExtent2D size);

void dynamic_rendertarget_recreate(DynamicCPUTarget* target, VkExtent2D extent);

}
