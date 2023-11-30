#include "swapchain.hpp"

#include "graphics.hpp"

#include "device.hpp"

#include "yolo/yolo.hpp"

namespace inferno::graphics {

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR Capabilities;
    std::vector<VkSurfaceFormatKHR> Formats;
    std::vector<VkPresentModeKHR> PresentModes;
};

SwapChainSupportDetails device_get_swapchain_support(
    GraphicsDevice* g, VkPhysicalDevice device)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        device, g->VulkanSurface, &details.Capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, g->VulkanSurface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.Formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            device, g->VulkanSurface, &formatCount, details.Formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, g->VulkanSurface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.PresentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device, g->VulkanSurface, &presentModeCount, details.PresentModes.data());
    }

    return details;
}

void create_image(GraphicsDevice* device, uint32_t width, uint32_t height,
    VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
    VkImageCreateInfo imageInfo {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(device->VulkanDevice, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device->VulkanDevice, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex
        = device_find_memory_type(device, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device->VulkanDevice, &allocInfo, nullptr, &imageMemory)
        != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(device->VulkanDevice, image, imageMemory, 0);
}

VkImageView create_image_view(GraphicsDevice* device, VkImage image, VkFormat format,
    VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewInfo {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(device->VulkanDevice, &viewInfo, nullptr, &imageView)
        != VK_SUCCESS) {
        yolo::error("failed to create texture image view!");
    }

    return imageView;
}

VkFormat find_format(GraphicsDevice* device, const std::vector<VkFormat>& candidates,
    VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(device->VulkanPhysicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR
            && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL
            && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

VkFormat find_depth_format(GraphicsDevice* device)
{
    // depth and stencil format
    return VK_FORMAT_D32_SFLOAT_S8_UINT;
    // return find_format(device, { VK_FORMAT_D, VK_FORMAT_D32_SFLOAT_S8_UINT,
    //                                VK_FORMAT_D24_UNORM_S8_UINT },
    //     VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkSurfaceFormatKHR device_choose_swap_surface_format(
    const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
            && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR device_choose_swap_present_mode(
    const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D device_choose_swap_extent(
    const VkSurfaceCapabilitiesKHR& capabilities, int width, int height)
{
    if (capabilities.currentExtent.width != UINT32_MAX) {
        yolo::info("Surface size: {}x{}", capabilities.currentExtent.width,
            capabilities.currentExtent.height);
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = { (uint32_t)width, (uint32_t)height };
        yolo::info("Surface size: {}x{}", width, height);

        actualExtent.width = std::max(capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width, actualExtent.width));

        actualExtent.height = std::max(capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

SwapChain* swapchain_create(GraphicsDevice* device, glm::ivec2 surface_size)
{
    SwapChain* swapchain = new SwapChain();

    SwapChainSupportDetails swapChainSupport
        = device_get_swapchain_support(device, device->VulkanPhysicalDevice);

    VkSurfaceFormatKHR surfaceFormat
        = device_choose_swap_surface_format(swapChainSupport.Formats);
    VkPresentModeKHR presentMode
        = device_choose_swap_present_mode(swapChainSupport.PresentModes);
    yolo::debug("Surface format: {}", surfaceFormat.format);
    VkExtent2D extent = device_choose_swap_extent(
        swapChainSupport.Capabilities, surface_size.x, surface_size.y);
    swapchain->Extent = extent;
    uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;

    if (swapChainSupport.Capabilities.maxImageCount > 0
        && imageCount > swapChainSupport.Capabilities.maxImageCount) {
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

    QueueFamilyIndices indices
        = device_get_queue_families(device, device->VulkanPhysicalDevice);
    uint32_t queueFamilyIndices[]
        = { indices.graphicsFamily.value(), indices.presentFamily.value() };

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

    if (vkCreateSwapchainKHR(
            device->VulkanDevice, &createInfo, nullptr, &swapchain->Handle)
        != VK_SUCCESS) {
        yolo::error("failed to create swap chain!");
        exit(1);
    }
    yolo::info("Swap chain created");

    vkGetSwapchainImagesKHR(
        device->VulkanDevice, swapchain->Handle, &imageCount, nullptr);
    swapchain->Images.resize(imageCount);
    vkGetSwapchainImagesKHR(
        device->VulkanDevice, swapchain->Handle, &imageCount, swapchain->Images.data());

    swapchain->ImageFormat = surfaceFormat.format;
    swapchain->Device = device;

    swapchain_image_view_create(swapchain);

    return swapchain;
}

void swapchain_cleanup(SwapChain* swapchain)
{
    for (auto imageView : swapchain->ImageViews) {
        vkDestroyImageView(swapchain->Device->VulkanDevice, imageView, nullptr);
    }
    vkDestroySwapchainKHR(swapchain->Device->VulkanDevice, swapchain->Handle, nullptr);
    delete swapchain;
}

void swapchain_image_view_create(SwapChain* swapchain)
{
    swapchain->ImageViews.resize(swapchain->Images.size());

    for (size_t i = 0; i < swapchain->Images.size(); i++) {
        swapchain->ImageViews[i] = create_image_view(swapchain->Device,
            swapchain->Images[i], swapchain->ImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    VkFormat depthFormat = find_depth_format(swapchain->Device);
    swapchain->DepthFormat = depthFormat;

    create_image(swapchain->Device, swapchain->Extent.width, swapchain->Extent.height,
        depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, swapchain->depthImage,
        swapchain->depthImageMemory);
    swapchain->DepthImageView = create_image_view(
        swapchain->Device, swapchain->depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void swapchain_recreate(SwapChain* swapchain)
{
    vkDeviceWaitIdle(swapchain->Device->VulkanDevice);
    GraphicsDevice* device = swapchain->Device;

    swapchain_cleanup(swapchain);
    swapchain = swapchain_create(device, device->SurfaceSize);
}

}
