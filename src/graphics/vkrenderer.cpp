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

    renderer->CommandBuffers.resize(FRAMES_IN_FLIGHT);
    renderer->ImageAvailableSemaphores.resize(FRAMES_IN_FLIGHT);
    renderer->RenderFinishedSemaphores.resize(FRAMES_IN_FLIGHT);
    renderer->InFlightFences.resize(FRAMES_IN_FLIGHT);

    // Creating the synchronization objects
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Start in signaled state

    for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(device->VulkanDevice, &semaphoreInfo, nullptr,
                &renderer->ImageAvailableSemaphores[i])
                != VK_SUCCESS
            || vkCreateSemaphore(device->VulkanDevice, &semaphoreInfo, nullptr,
                   &renderer->RenderFinishedSemaphores[i])
                != VK_SUCCESS
            || vkCreateFence(device->VulkanDevice, &fenceInfo, nullptr,
                   &renderer->InFlightFences[i])
                != VK_SUCCESS) {
            yolo::error("failed to create synchronization objects for a frame!");
        }
    }

    return renderer;
}

void renderer_cleanup(VulkanRenderer* renderer)
{
    vkDestroyCommandPool(renderer->Device->VulkanDevice, renderer->CommandPool, nullptr);
    for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(renderer->Device->VulkanDevice,
            renderer->ImageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(renderer->Device->VulkanDevice,
            renderer->RenderFinishedSemaphores[i], nullptr);
        vkDestroyFence(
            renderer->Device->VulkanDevice, renderer->InFlightFences[i], nullptr);
    }
}

void renderer_configure_command_buffer(Renderer* renderer)
{
    QueueFamilyIndices i = device_get_queue_families(
        renderer->Device, renderer->Device->VulkanPhysicalDevice);
    VkCommandPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = i.graphicsFamily.value();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(
            renderer->Device->VulkanDevice, &poolInfo, nullptr, &renderer->CommandPool)
        != VK_SUCCESS) {
        yolo::error("failed to create command pool!");
    }

    renderer->CommandBuffers.resize(FRAMES_IN_FLIGHT);
    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = renderer->CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)renderer->CommandBuffers.size();

    if (vkAllocateCommandBuffers(
            renderer->Device->VulkanDevice, &allocInfo, &renderer->CommandBuffers[0])
        != VK_SUCCESS) {
        yolo::error("failed to allocate command buffers!");
    }

    yolo::debug("Command buffer created");
}

void renderer_record_command_buffer(
    Renderer* renderer, RenderPass* renderpass, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(renderer->CommandBuffers[renderer->CurrentFrame], &beginInfo) != VK_SUCCESS) {
        yolo::error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderpass->VulkanRenderPass;
    renderPassInfo.framebuffer
        = renderpass->RenderPipeline->Swap->SwapFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = renderpass->RenderPipeline->Swap->Extent;

    VkClearValue clearColor = { { { 0.0f, 0.3f, 0.3f, 1.0f } } };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(
        renderer->CommandBuffers[renderer->CurrentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void renderer_begin_frame(VulkanRenderer* renderer, RenderPass* renderpass)
{
    vkWaitForFences(
        renderer->Device->VulkanDevice, 1, &renderer->InFlightFences[renderer->CurrentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(renderer->Device->VulkanDevice, 1, &renderer->InFlightFences[renderer->CurrentFrame]);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(renderer->Device->VulkanDevice, renderer->Swap->Handle,
        UINT64_MAX, renderer->ImageAvailableSemaphores[renderer->CurrentFrame], VK_NULL_HANDLE, &imageIndex);
    renderer->ImageIndex = imageIndex;

    vkResetCommandBuffer(renderer->CommandBuffers[renderer->CurrentFrame], 0);
    renderer_record_command_buffer(renderer, renderpass, imageIndex);
}

void renderer_draw_frame(Renderer* renderer, RenderPass* renderpass)
{
    vkCmdEndRenderPass(renderer->CommandBuffers[renderer->CurrentFrame]);
    if (vkEndCommandBuffer(renderer->CommandBuffers[renderer->CurrentFrame]) != VK_SUCCESS) {
        yolo::error("failed to record command buffer!");
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { renderer->ImageAvailableSemaphores[renderer->CurrentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask
        = waitStages; // TODO: This is a bit of a hack, we should be waiting for the
                      // renderpass to finish, not the color attachment stage

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &renderer->CommandBuffers[renderer->CurrentFrame];

    VkSemaphore signalSemaphores[] = { renderer->RenderFinishedSemaphores[renderer->CurrentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(renderer->Device->VulkanGraphicsQueue, 1, &submitInfo,
            renderer->InFlightFences[renderer->CurrentFrame])
        != VK_SUCCESS) {
        yolo::error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { renderer->Swap->Handle };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &renderer->ImageIndex;

    vkQueuePresentKHR(renderer->Device->VulkanPresentQueue, &presentInfo);

    renderer->CurrentFrame = (renderer->CurrentFrame + 1) % FRAMES_IN_FLIGHT;
}

}
