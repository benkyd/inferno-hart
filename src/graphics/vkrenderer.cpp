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

    renderer->InFlight.resize(FRAMES_IN_FLIGHT);

    // Creating the synchronization objects
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Start in signaled state

    for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
        renderer->InFlight[i] = FrameInFlight{};
        // Create the Uniform Buffer here

        if (vkCreateSemaphore(device->VulkanDevice, &semaphoreInfo, nullptr,
                &renderer->InFlight[i].ImageAvailable)
                != VK_SUCCESS
            || vkCreateSemaphore(device->VulkanDevice, &semaphoreInfo, nullptr,
                   &renderer->InFlight[i].RenderFinished)
                != VK_SUCCESS
            || vkCreateFence(device->VulkanDevice, &fenceInfo, nullptr,
                   &renderer->InFlight[i].Fence)
                != VK_SUCCESS) {
            yolo::error("failed to create synchronization objects for a frame!");
        }
    }

    renderer->CurrentFrameIndex = 0;
    renderer->CurrentFrame = &renderer->InFlight[0];

    return renderer;
}

void renderer_cleanup(VulkanRenderer* renderer)
{
    vkDestroyCommandPool(
        renderer->Device->VulkanDevice, renderer->Device->VulkanCommandPool, nullptr);
    for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(renderer->Device->VulkanDevice,
            renderer->InFlight[i].ImageAvailable, nullptr);
        vkDestroySemaphore(renderer->Device->VulkanDevice,
            renderer->InFlight[i].RenderFinished, nullptr);
        vkDestroyFence(
            renderer->Device->VulkanDevice, renderer->InFlight[i].Fence, nullptr);
    }
}

void renderer_configure_command_buffer(Renderer* renderer)
{
    renderer->CommandBuffersInFlight.resize(FRAMES_IN_FLIGHT);
    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = renderer->Device->VulkanCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)FRAMES_IN_FLIGHT;

    if (vkAllocateCommandBuffers(renderer->Device->VulkanDevice, &allocInfo,
            &renderer->CommandBuffersInFlight[0])
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

    if (vkBeginCommandBuffer(
            renderer->CommandBuffersInFlight[renderer->CurrentFrameIndex], &beginInfo)
        != VK_SUCCESS) {
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

    vkCmdBeginRenderPass(renderer->CommandBuffersInFlight[renderer->CurrentFrameIndex],
        &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

bool renderer_begin_frame(VulkanRenderer* renderer, RenderPass* renderpass)
{
    vkWaitForFences(renderer->Device->VulkanDevice, 1, &renderer->CurrentFrame->Fence,
        VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    auto swapStatus = vkAcquireNextImageKHR(renderer->Device->VulkanDevice,
        renderer->Swap->Handle, UINT64_MAX, renderer->CurrentFrame->ImageAvailable,
        VK_NULL_HANDLE, &imageIndex);
    renderer->ImageIndex = imageIndex;

    if (swapStatus == VK_ERROR_OUT_OF_DATE_KHR || renderer->Device->Resized) {
        yolo::info("Swapchain out of date");
        swapchain_recreate(renderer->Swap);
        return false;
    } else if (swapStatus != VK_SUCCESS) {
        yolo::error("failed to acquire swap chain image!");
    }

    vkResetFences(renderer->Device->VulkanDevice, 1,
        &renderer->CurrentFrame->Fence);
    vkResetCommandBuffer(renderer->CommandBuffersInFlight[renderer->CurrentFrameIndex], 0);
    renderer_record_command_buffer(renderer, renderpass, imageIndex);
    return true;
}

bool renderer_draw_frame(Renderer* renderer, RenderPass* renderpass)
{
    vkCmdEndRenderPass(renderer->CommandBuffersInFlight[renderer->CurrentFrameIndex]);
    if (vkEndCommandBuffer(renderer->CommandBuffersInFlight[renderer->CurrentFrameIndex])
        != VK_SUCCESS) {
        yolo::error("failed to record command buffer!");
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[]
        = { renderer->CurrentFrame->ImageAvailable };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask
        = waitStages; // TODO: This is a bit of a hack, we should be waiting for the
                      // renderpass to finish, not the color attachment stage

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &renderer->CommandBuffersInFlight[renderer->CurrentFrameIndex];

    VkSemaphore signalSemaphores[]
        = { renderer->CurrentFrame->RenderFinished };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(renderer->Device->VulkanGraphicsQueue, 1, &submitInfo,
            renderer->CurrentFrame->Fence)
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

    auto swapStatus
        = vkQueuePresentKHR(renderer->Device->VulkanPresentQueue, &presentInfo);

    if (swapStatus == VK_ERROR_OUT_OF_DATE_KHR || swapStatus == VK_SUBOPTIMAL_KHR
        || renderer->Device->Resized) {
        yolo::info("Swapchain out of date");
        swapchain_recreate(renderer->Swap);
    } else if (swapStatus != VK_SUCCESS) {
        yolo::error("failed to present swap chain image!");
    }

    renderer->CurrentFrameIndex = (renderer->CurrentFrameIndex + 1) % FRAMES_IN_FLIGHT;
    renderer->CurrentFrame = &renderer->InFlight[renderer->CurrentFrameIndex];
    return true;
}

}
