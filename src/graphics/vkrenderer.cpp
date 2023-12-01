#include "vkrenderer.hpp"

#include "device.hpp"
#include "graphics.hpp"
#include "pipeline.hpp"
#include "swapchain.hpp"

#include "gui/gui.hpp"

#include "yolo/yolo.hpp"
#include <functional>
#include <vulkan/vulkan_core.h>

namespace inferno::graphics {

VulkanRenderer* renderer_create(GraphicsDevice* device)
{
    auto renderer = new VulkanRenderer();
    renderer->Device = device;
    renderer->Swap = swapchain_create(device, device->SurfaceSize);

    renderer->InFlight.resize(FRAMES_IN_FLIGHT);

    // Creating the synchronization objects
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Start in signaled state

    for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
        renderer->InFlight[i] = FrameInFlight {};
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

    gui::imgui_init(renderer);

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

void renderer_configure_command_buffer(VulkanRenderer* renderer)
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

void renderer_record_command_buffer(VulkanRenderer* renderer, uint32_t imageIndex)
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

    renderer->CommandBufferInFlight
        = &renderer->CommandBuffersInFlight[renderer->CurrentFrameIndex];

    VkImageMemoryBarrier imageMemoryBarrier {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    imageMemoryBarrier.image = renderer->Swap->Images[renderer->ImageIndex];
    imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.layerCount = 1;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.levelCount = 1;

    vkCmdPipelineBarrier(renderer->CommandBuffersInFlight[renderer->CurrentFrameIndex],
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

    VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
    VkRenderingAttachmentInfo attachmentInfo {};
    attachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    attachmentInfo.imageView = renderer->Swap->ImageViews[imageIndex];
    attachmentInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
    attachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentInfo.clearValue = clearColor;

    VkRenderingAttachmentInfo depthAttachmentInfo;
    depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    depthAttachmentInfo.imageView = renderer->Swap->DepthImageView;
    depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
    depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachmentInfo.clearValue.depthStencil = { 1.0f, 0 };
    depthAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
    depthAttachmentInfo.resolveImageView = VK_NULL_HANDLE;
    depthAttachmentInfo.resolveImageLayout = VK_IMAGE_LAYOUT_GENERAL;
    depthAttachmentInfo.pNext = VK_NULL_HANDLE;

    VkRenderingInfo renderingInfo {};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    renderingInfo.renderArea
        = { 0, 0, renderer->Swap->Extent.width, renderer->Swap->Extent.height };
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &attachmentInfo;
    renderingInfo.pDepthAttachment = &depthAttachmentInfo;

    vkCmdBeginRendering(
        renderer->CommandBuffersInFlight[renderer->CurrentFrameIndex], &renderingInfo);
}

void renderer_submit_oneoff(VulkanRenderer* renderer,
    std::function<void(VulkanRenderer*, VkCommandBuffer*)> callback, bool post)
{
    if (post) {
        renderer->SubmitQueueOneOffPostFrame.push_back(callback);
    } else {
        renderer->SubmitQueueOneOffPreFrame.push_back(callback);
    }
}

void renderer_submit_repeat(VulkanRenderer* renderer,
    std::function<void(VulkanRenderer*, VkCommandBuffer*)> callback, bool post)
{
    if (post) {
        renderer->SubmitQueuePostFrame.push_back(callback);
    } else {
        renderer->SubmitQueuePreFrame.push_back(callback);
    }
}

void work_queue(VulkanRenderer* renderer,
    std::vector<std::function<void(VulkanRenderer*, VkCommandBuffer*)>>* queue,
    bool clear)
{
    for (auto& callback : *queue) {
        callback(renderer, renderer->CommandBufferInFlight);
    }
    if (clear) {
        queue->clear();
    }
}

void renderer_submit_now(VulkanRenderer* renderer,
    std::function<void(VulkanRenderer*, VkCommandBuffer*)> callback)
{
    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = renderer->Device->VulkanCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(renderer->Device->VulkanDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    callback(renderer, &commandBuffer);

    vkEndCommandBuffer(commandBuffer);
    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(renderer->Device->VulkanGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(renderer->Device->VulkanGraphicsQueue);

    vkFreeCommandBuffers(renderer->Device->VulkanDevice,
        renderer->Device->VulkanCommandPool, 1, &commandBuffer);
}

bool renderer_begin_frame(VulkanRenderer* renderer)
{
    vkWaitForFences(renderer->Device->VulkanDevice, 1, &renderer->CurrentFrame->Fence,
        VK_TRUE, UINT64_MAX);

    auto swapStatus = vkAcquireNextImageKHR(renderer->Device->VulkanDevice,
        renderer->Swap->Handle, UINT64_MAX, renderer->CurrentFrame->ImageAvailable,
        VK_NULL_HANDLE, &renderer->ImageIndex);

    if (swapStatus == VK_ERROR_OUT_OF_DATE_KHR || renderer->Device->Resized) {
        yolo::info("Swapchain out of date");
        swapchain_recreate(renderer->Swap);
        return false;
    } else if (swapStatus != VK_SUCCESS) {
        yolo::error("failed to acquire swap chain image!");
    }

    vkResetFences(renderer->Device->VulkanDevice, 1, &renderer->CurrentFrame->Fence);
    vkResetCommandBuffer(
        renderer->CommandBuffersInFlight[renderer->CurrentFrameIndex], 0);
    renderer_record_command_buffer(renderer, renderer->ImageIndex);

    work_queue(renderer, &renderer->SubmitQueueOneOffPreFrame, true);
    work_queue(renderer, &renderer->SubmitQueuePreFrame, false);

    gui::imgui_new_frame();


    return true;
}

bool renderer_draw_frame(VulkanRenderer* renderer)
{
    work_queue(renderer, &renderer->SubmitQueueOneOffPreFrame, true);
    work_queue(renderer, &renderer->SubmitQueuePreFrame, false);


    // ImGui::ShowDemoWindow();

    gui::imgui_render_frame(*renderer->CommandBufferInFlight);

    vkCmdEndRendering(*renderer->CommandBufferInFlight);

    VkImageMemoryBarrier imageMemoryBarrier {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    imageMemoryBarrier.image = renderer->Swap->Images[renderer->ImageIndex];
    imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.layerCount = 1;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.levelCount = 1;

    vkCmdPipelineBarrier(renderer->CommandBuffersInFlight[renderer->CurrentFrameIndex],
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1,
        &imageMemoryBarrier);


    if (vkEndCommandBuffer(renderer->CommandBuffersInFlight[renderer->CurrentFrameIndex])
        != VK_SUCCESS) {
        yolo::error("failed to record command buffer!");
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { renderer->CurrentFrame->ImageAvailable };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask
        = waitStages; // TODO: This is a bit of a hack, we should be waiting for the
                      // renderpass to finish, not the color attachment stage

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers
        = &renderer->CommandBuffersInFlight[renderer->CurrentFrameIndex];

    VkSemaphore signalSemaphores[] = { renderer->CurrentFrame->RenderFinished };
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
