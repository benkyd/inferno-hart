#include "swapchain.hpp"

#include "device.hpp"

#include "yolo/yolo.hpp"

namespace inferno::graphics {

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR Capabilities;
    std::vector<VkSurfaceFormatKHR> Formats;
    std::vector<VkPresentModeKHR> PresentModes;
};

SwapChainSupportDetails device_get_swapchain_support(GraphicsDevice* g, VkPhysicalDevice device)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, g->VulkanSurface, &details.Capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, g->VulkanSurface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.Formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, g->VulkanSurface, &formatCount, details.Formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, g->VulkanSurface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.PresentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, g->VulkanSurface, &presentModeCount, details.PresentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR device_choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (
            availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR device_choose_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D device_choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height)
{
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = { (uint32_t)width, (uint32_t)height };

        actualExtent.width = std::max(
            capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width, actualExtent.width));

        actualExtent.height = std::max(
            capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

SwapChain* swapchain_create(GraphicsDevice* device, glm::ivec2 surface_size)
{
    SwapChain* swapchain = new SwapChain();

    SwapChainSupportDetails swapChainSupport = device_get_swapchain_support(device, device->VulkanPhysicalDevice);

    VkSurfaceFormatKHR surfaceFormat = device_choose_swap_surface_format(swapChainSupport.Formats);
    VkPresentModeKHR presentMode = device_choose_swap_present_mode(swapChainSupport.PresentModes);
    VkExtent2D extent = device_choose_swap_extent(swapChainSupport.Capabilities, surface_size.x, surface_size.y);
    uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;

    if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount) {
        imageCount = swapChainSupport.Capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = device->VulkanSurface;
    createInfo.minImageCount = imageCount;

    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = device_get_queue_families(device, device->VulkanPhysicalDevice);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device->VulkanDevice, &createInfo, nullptr, &swapchain->Handle) != VK_SUCCESS) {
        yolo::error("failed to create swap chain!");
        exit(1);
    }
    yolo::info("Swap chain created");

    vkGetSwapchainImagesKHR(device->VulkanDevice, swapchain->Handle, &imageCount, nullptr);
    swapchain->Images.resize(imageCount);
    vkGetSwapchainImagesKHR(device->VulkanDevice, swapchain->Handle, &imageCount, swapchain->Images.data());

    swapchain->ImageFormat = surfaceFormat.format;
    swapchain->Extent = extent;
    swapchain->Device = device;

    swapchain_image_view_create(swapchain);

    return swapchain;
}

void swapchain_cleanup(SwapChain* swapchain)
{
    vkDestroySwapchainKHR(swapchain->Device->VulkanDevice, swapchain->Handle, nullptr);
    delete swapchain;
}

void swapchain_image_view_create(SwapChain* swapchain)
{
    swapchain->ImageViews.resize(swapchain->Images.size());

    for (size_t i = 0; i < swapchain->Images.size(); i++) {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapchain->Images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapchain->ImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(swapchain->Device->VulkanDevice, &createInfo, nullptr, &swapchain->ImageViews[i]) != VK_SUCCESS) {
            yolo::error("failed to create image views!");
            exit(1);
        }
    }
}

}
