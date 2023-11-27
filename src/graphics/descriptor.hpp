#pragma once

#include "graphics.hpp"

namespace inferno::graphics {

struct GraphicsDevice;

typedef struct DescriptorSet {
    VkDescriptorSet Set;
    VkDescriptorSetLayout Layout;
} DescriptorSet;

typedef struct DescriptorPool {
    VkDescriptorPool Pool;
    std::vector<VkDescriptorSet> Sets;
} DescriptorPool;

typedef struct Descriptor {
    VkDescriptorType Type;
    VkShaderStageFlags StageFlags;
    uint32_t Binding;
    uint32_t Count;
} Descriptor;

// reading docs ...

DescriptorPool* descriptor_pool_create(GraphicsDevice* device, uint32_t max_sets, std::vector<Descriptor> descriptors);
void descriptor_pool_cleanup(DescriptorPool* pool);

DescriptorSet* descriptor_set_create(GraphicsDevice* device, DescriptorPool* pool, std::vector<Descriptor> descriptors);
void descriptor_set_cleanup(DescriptorSet* set);

}

