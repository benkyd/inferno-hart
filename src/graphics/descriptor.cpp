#include "descriptor.hpp"

#include "device.hpp"
#include "graphics.hpp"

#include "yolo/yolo.hpp"

namespace inferno::graphics {

DescriptorPool* descriptor_pool_create(GraphicsDevice* device, uint32_t max_sets)
{
    DescriptorPool* pool = new DescriptorPool();
    pool->Device = device;
    pool->Sets.resize(max_sets);

    VkDescriptorPoolSize poolSize {};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = max_sets;

    VkDescriptorPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = max_sets;

    if (vkCreateDescriptorPool(device->VulkanDevice, &poolInfo, nullptr, &pool->Handle)
        != VK_SUCCESS) {
        yolo::error("failed to create descriptor pool!");
    }

    return pool;
}

void descriptor_pool_cleanup(DescriptorPool* pool)
{
    vkDestroyDescriptorPool(pool->Device->VulkanDevice, pool->Handle, nullptr);
    delete pool;
}



}
