#pragma once

#include "graphics.hpp"

namespace inferno::scene {
struct Vert;
}

namespace inferno::graphics {
struct GraphicsDevice;

typedef struct VertexBuffer {
    GraphicsDevice* Device;

    VkBuffer Handle;
    VkDeviceMemory DeviceData;

    uint32_t Size;
    void* NullableClientData;
} VertexBuffer;

VertexBuffer* vertex_buffer_create(GraphicsDevice* device, void* data, uint32_t size);
void vertex_buffer_cleanup(VertexBuffer* buffer);

void vertex_buffer_bind(VertexBuffer* buffer, VkCommandBuffer commandBuffer);

}
