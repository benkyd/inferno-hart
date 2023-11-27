#pragma once

#include "graphics.hpp"

namespace inferno::graphics {

struct GraphicsDevice;

// NOTE: THIS IS NOT THE RIGHT WAY TO DO THIS

typedef struct DescriptorSet {
    GraphicsDevice* Device;
    VkDescriptorSet Handle;
    VkDescriptorSetLayout Layout;
} DescriptorSet;

typedef struct DescriptorPool {
    GraphicsDevice* Device;
    VkDescriptorPool Handle;
    std::vector<DescriptorSet> Sets;
} DescriptorPool;

DescriptorPool* descriptor_pool_create(GraphicsDevice* device, uint32_t max_sets);
void descriptor_pool_cleanup(DescriptorPool* pool);

DescriptorSet* descriptor_set_create(GraphicsDevice* device, DescriptorPool* pool);
void descriptor_set_cleanup(DescriptorSet* set);

}
