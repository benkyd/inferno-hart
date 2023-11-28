#pragma once

#include "graphics.hpp"

namespace inferno::scene {
struct Vert;
}

namespace inferno::graphics {
struct GraphicsDevice;

typedef struct GenBuffer {
    GraphicsDevice* Device;

    VkBuffer Handle;
    VkDeviceMemory DeviceData;

    void* MappedData;

    uint32_t Count;
    VkDeviceSize Size;
} GenBuffer;

// NOTE: Staging Buffer and Buffer are literally the exact same thing
// It's all about the semantics of on-device memory transfer
// TODO: Does the size of the staging buffer matter?
typedef struct Buffer {
    GenBuffer* GenericBuffer;
    GenBuffer* StagingBuffer;
    void* NullableClientData;
} Buffer;

GenBuffer* generic_buffer_create(GraphicsDevice* device, uint32_t count,
    VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
void generic_buffer_cleanup(GenBuffer* buffer);

void buffer_copy(Buffer* buffer, GraphicsDevice* device);

Buffer* vertex_buffer_create(GraphicsDevice* device, void* data, uint32_t size, bool bind = false);
void vertex_buffer_cleanup(Buffer* buffer);
void vertex_buffer_bind(Buffer* buffer, VkCommandBuffer commandBuffer);

Buffer* index_buffer_create(GraphicsDevice* device, void* data, uint32_t size, bool bind = false);
void index_buffer_cleanup(Buffer* buffer);
void index_buffer_bind(Buffer* buffer, VkCommandBuffer commandBuffer);

// We *do* want universally mapped memory to act as a "uniform buffer"
template<typename T> GenBuffer* uniform_buffer_create(GraphicsDevice* device, bool bind = false);
void uniform_buffer_cleanup(GenBuffer* buffer);
template<typename T> void uniform_buffer_update(GenBuffer* buffer, T* data);

}
