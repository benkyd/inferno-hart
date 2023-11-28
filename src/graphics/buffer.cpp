#include "buffer.hpp"

#include "device.hpp"

#include "scene/mesh.hpp"

#include "yolo/yolo.hpp"
#include <vulkan/vulkan_core.h>

namespace inferno::graphics {

GenBuffer* generic_buffer_create(GraphicsDevice* device, uint32_t count,
    VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
    GenBuffer* buffer = new GenBuffer;

    buffer->Device = device;
    buffer->Count = count;
    buffer->Size = size;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
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
        = device_find_memory_type(device, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device->VulkanDevice, &allocInfo, nullptr, &buffer->DeviceData)
        != VK_SUCCESS) {
        yolo::error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(device->VulkanDevice, buffer->Handle, buffer->DeviceData, 0);

    return buffer;
}

void generic_buffer_cleanup(GenBuffer* buffer)
{
    vkDestroyBuffer(buffer->Device->VulkanDevice, buffer->Handle, nullptr);
    vkFreeMemory(buffer->Device->VulkanDevice, buffer->DeviceData, nullptr);
    delete buffer;
}

void buffer_copy(Buffer* buffer, GraphicsDevice* device)
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    // shit this needs command pool access
    allocInfo.commandPool = device->VulkanCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device->VulkanDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = buffer->StagingBuffer->Size;

    vkCmdCopyBuffer(commandBuffer, buffer->StagingBuffer->Handle,
        buffer->GenericBuffer->Handle, 1, &copyRegion);
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(device->VulkanGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device->VulkanGraphicsQueue);

    vkFreeCommandBuffers(
        device->VulkanDevice, device->VulkanCommandPool, 1, &commandBuffer);
}

Buffer* vertex_buffer_create(GraphicsDevice* device, void* data, uint32_t size)
{
    VkDeviceSize bufferSize = size * sizeof(scene::Vert);
    Buffer* buffer = new Buffer;
    buffer->StagingBuffer = generic_buffer_create(device, size, bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    buffer->NullableClientData = data;

    void* mData;
    vkMapMemory(device->VulkanDevice, buffer->StagingBuffer->DeviceData, 0, bufferSize, 0,
        &mData);
    memcpy(mData, data, bufferSize);
    vkUnmapMemory(device->VulkanDevice, buffer->StagingBuffer->DeviceData);

    buffer->GenericBuffer = generic_buffer_create(device, size, bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    buffer_copy(buffer, device);

    return buffer;
}

void vertex_buffer_cleanup(Buffer* buffer)
{
    generic_buffer_cleanup(buffer->GenericBuffer);
    generic_buffer_cleanup(buffer->StagingBuffer);
    delete buffer;
}

void vertex_buffer_bind(Buffer* buffer, VkCommandBuffer commandBuffer)
{
    VkBuffer vertexBuffers[] = { buffer->GenericBuffer->Handle };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
}

Buffer* index_buffer_create(GraphicsDevice* device, void* data, uint32_t size)
{
    VkDeviceSize bufferSize = size * sizeof(scene::Index);
    Buffer* buffer = new Buffer;
    buffer->StagingBuffer = generic_buffer_create(device, size, bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    buffer->NullableClientData = data;

    void* mData;
    vkMapMemory(device->VulkanDevice, buffer->StagingBuffer->DeviceData, 0, bufferSize, 0,
        &mData);
    memcpy(mData, data, bufferSize);
    vkUnmapMemory(device->VulkanDevice, buffer->StagingBuffer->DeviceData);

    buffer->GenericBuffer = generic_buffer_create(device, size, bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    buffer_copy(buffer, device);

    return buffer;
}

void index_buffer_cleanup(Buffer* buffer)
{
    generic_buffer_cleanup(buffer->GenericBuffer);
    generic_buffer_cleanup(buffer->StagingBuffer);
    delete buffer;
}

void index_buffer_bind(Buffer* buffer, VkCommandBuffer commandBuffer)
{
    vkCmdBindIndexBuffer(
        commandBuffer, buffer->GenericBuffer->Handle, 0, VK_INDEX_TYPE_UINT32);
}

template <typename T> GenBuffer* uniform_buffer_create(GraphicsDevice* device, bool bind)
{
    GenBuffer* buffer
        = generic_buffer_create(device, 0, sizeof(T), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkMapMemory(device->VulkanDevice, buffer->DeviceData, 0, buffer->Size, 0,
        &buffer->MappedData);

    return buffer;
}

void uniform_buffer_cleanup(GenBuffer* buffer) { generic_buffer_cleanup(buffer); }

template <typename T> void uniform_buffer_update(GenBuffer* buffer, T* data)
{
    memcpy(buffer->MappedData, (void*)data, sizeof(T));
}

}
