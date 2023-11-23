#include "vkrenderer.hpp"

#include "device.hpp"
#include "graphics.hpp"
#include "pipeline.hpp"
#include "renderpass.hpp"
#include "swapchain.hpp"

#include "yolo/yolo.hpp"

namespace inferno::graphics {

VulkanRenderer* renderer_create(GraphicsDevice* device, SwapChain* swapchain)
{
    auto renderer = new VulkanRenderer();
    renderer->Device = device;
    renderer->Swap = swapchain;

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Start in signaled state

    if (vkCreateSemaphore(device->VulkanDevice, &semaphoreInfo, nullptr,
            &renderer->ImageAvailableSemaphore)
            != VK_SUCCESS
        || vkCreateSemaphore(device->VulkanDevice, &semaphoreInfo, nullptr,
               &renderer->RenderFinishedSemaphore)
            != VK_SUCCESS
        || vkCreateFence(
               device->VulkanDevice, &fenceInfo, nullptr, &renderer->InFlightFence)
            != VK_SUCCESS) {
        yolo::error("failed to create synchronization objects for a frame!");
    }

    return renderer;
}

void renderer_cleanup(VulkanRenderer* renderer)
{
    vkDestroySemaphore(
        renderer->Device->VulkanDevice, renderer->RenderFinishedSemaphore, nullptr);
    vkDestroySemaphore(
        renderer->Device->VulkanDevice, renderer->ImageAvailableSemaphore, nullptr);
    vkDestroyFence(renderer->Device->VulkanDevice, renderer->InFlightFence, nullptr);
}

void renderer_begin_frame(VulkanRenderer* renderer)
{
    vkWaitForFences(
        renderer->Device->VulkanDevice, 1, &renderer->InFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(renderer->Device->VulkanDevice, 1, &renderer->InFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(renderer->Device->VulkanDevice, renderer->Swap->Handle,
        UINT64_MAX, renderer->ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    // renderer->Swap->CurrentImageIndex = imageIndex;
}

}
