#include "buffer.hpp"

#include "device.hpp"

#include "scene/mesh.hpp"

#include "yolo/yolo.hpp"

namespace inferno::graphics {

VertexBuffer* vertex_buffer_create(GraphicsDevice* device, void* data, uint32_t size)
{
    VertexBuffer* buffer = new VertexBuffer;

    buffer->Device = device;
    buffer->Size = size;
    buffer->NullableClientData = data;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size * sizeof(scene::Vert);
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device->VulkanDevice, &bufferInfo, nullptr, &buffer->Handle)
        != VK_SUCCESS) {
        yolo::error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device->VulkanDevice, buffer->Handle, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex
        = device_find_memory_type(device, memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(device->VulkanDevice, &allocInfo, nullptr, &buffer->DeviceData)
        != VK_SUCCESS) {
        yolo::error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(device->VulkanDevice, buffer->Handle, buffer->DeviceData, 0);

    void* mData;
    vkMapMemory(device->VulkanDevice, buffer->DeviceData, 0, bufferInfo.size, 0, &mData);
    memcpy(mData, data, (size_t)bufferInfo.size);
    vkUnmapMemory(device->VulkanDevice, buffer->DeviceData);

    return buffer;
}

void vertex_buffer_cleanup(VertexBuffer* buffer)
{
    vkDestroyBuffer(buffer->Device->VulkanDevice, buffer->Handle, nullptr);
    vkFreeMemory(buffer->Device->VulkanDevice, buffer->DeviceData, nullptr);
    delete buffer;
}

void vertex_buffer_bind(VertexBuffer* buffer, VkCommandBuffer commandBuffer)
{
    VkBuffer vertexBuffers[] = { buffer->Handle };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
}

}
