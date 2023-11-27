#include "device.hpp"

#include "graphics.hpp"
#include "window.hpp"

#include "yolo/yolo.hpp"

#include <map>
#include <set>

namespace inferno::graphics {

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    yolo::warn("[VULKAN] {}", pCallbackData->pMessage);

    return VK_FALSE;
}

VkResult createDebugUtilsMessengerEXT(VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroyDebugUtilsMessengerEXT(VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

bool checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : VALIDATION_LAYERS) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

std::vector<const char*> getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(
        glfwExtensions, glfwExtensions + glfwExtensionCount);

    if constexpr (VALIDATION_LAYERS_ENABLED) {
        yolo::info("Validation layers enabled");
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    yolo::info("Requested instance extensions:");
    for (const auto& extension : extensions) {
        yolo::info("\t{}", extension);
    }

    return extensions;
}

QueueFamilyIndices device_get_queue_families(GraphicsDevice* g, VkPhysicalDevice device)
{
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
        device, &queueFamilyCount, queueFamilies.data());

    uint32_t queueFamIndex = 0;

    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = queueFamIndex;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            device, queueFamIndex, g->VulkanSurface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = queueFamIndex;
        }

        if (indices.isComplete()) {
            break;
        }

        queueFamIndex++;
    }

    return indices;
}

bool device_evaluate_extensions(
    VkPhysicalDevice device, std::vector<const char*> extensions)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(
        device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

bool device_evaluate(GraphicsDevice* g, VkPhysicalDevice device)
{
    int score = 0;

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    yolo::info("Found device {}", deviceProperties.deviceName);
    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        yolo::info("Device {} is a discrete GPU", deviceProperties.deviceName);
        score += 1000;
    }

    // does the device support the extensions we need?
    if (!device_evaluate_extensions(device, DEVICE_EXTENSIONS)) {
        return 0;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    // Application won't function without geometry shaders
    if (!deviceFeatures.geometryShader)
        return 0;

    // Ensure that the device can process the graphics commands that we need
    QueueFamilyIndices indices = device_get_queue_families(g, device);
    if (!indices.isComplete())
        return 0;

    return score;
}

GraphicsDevice* device_create()
{
    GraphicsDevice* device = new GraphicsDevice();
    device_create_vulkan_instance(device);
    device_vulkan_debugger(device);
    window_init_device(device, &device_resize_callback);
    device->SurfaceSize = window_get_size();
    device_create_vulkan_physical_device(device);
    device_create_vulkan_logical_device(device);
    device_create_command_pool(device);
    return device;
}

void device_cleanup(GraphicsDevice* device)
{
    vkDestroyDevice(device->VulkanDevice, nullptr);

    if constexpr (VALIDATION_LAYERS_ENABLED) {
        destroyDebugUtilsMessengerEXT(
            device->VulkanInstance, device->VulkanDebugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(device->VulkanInstance, device->VulkanSurface, nullptr);
    vkDestroyInstance(device->VulkanInstance, nullptr);
}

void device_create_vulkan_instance(GraphicsDevice* device)
{
    if (VALIDATION_LAYERS_ENABLED && !checkValidationLayerSupport()) {
        yolo::error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Inferno HART";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
#ifdef VALIDATION_LAYERS_ENABLED
    createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
    createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

    populateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#else
    createInfo.enabledLayerCount = 0;

    createInfo.pNext = nullptr;
#endif

    if (vkCreateInstance(&createInfo, nullptr, &device->VulkanInstance) != VK_SUCCESS) {
        yolo::error("failed to create instance!");
        exit(1);
    }
    yolo::info("Vulkan instance created");
}

void device_vulkan_debugger(GraphicsDevice* device)
{
    if constexpr (!VALIDATION_LAYERS_ENABLED) {
        yolo::warn("Validation layers disabled");
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (createDebugUtilsMessengerEXT(
            device->VulkanInstance, &createInfo, nullptr, &device->VulkanDebugMessenger)
        != VK_SUCCESS) {
        yolo::error("failed to set up debug messenger!");
        exit(1);
    }
    yolo::info("Vulkan debugger created");
}

void device_create_vulkan_physical_device(GraphicsDevice* device)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(device->VulkanInstance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        yolo::error("failed to find GPUs with Vulkan support!");
        exit(1);
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(device->VulkanInstance, &deviceCount, devices.data());

    // std::multimap is sorted list of key-value pairs. Sorted by key.
    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto& device_candidate : devices) {
        int score = device_evaluate(device, device_candidate);
        candidates.insert(std::make_pair(score, device_candidate));
    }

    // as candidates is a sorted list, the last value will always be the biggest score
    // (first element)
    if (candidates.rbegin()->first > 0) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(candidates.rbegin()->second, &deviceProperties);
        yolo::info("Using device: {}", deviceProperties.deviceName);

        // established the device with the best score, or the only one in the system.
        device->VulkanPhysicalDevice = candidates.rbegin()->second;
    } else {
        yolo::error("failed to find a suitable GPU!");
        exit(1);
    }
}

void device_create_vulkan_logical_device(GraphicsDevice* device)
{
    QueueFamilyIndices indices
        = device_get_queue_families(device, device->VulkanPhysicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies
        = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfo.queueCount = 1;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures {};

    VkDeviceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(DEVICE_EXTENSIONS.size());
    createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();
    yolo::info("Requested device extensions:");
    for (const auto& extension : DEVICE_EXTENSIONS) {
        yolo::info("\t{}", extension);
    }

    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicCreateInfo {};
    dynamicCreateInfo.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamicCreateInfo.dynamicRendering = VK_TRUE;
    createInfo.pNext = &dynamicCreateInfo;

#ifdef VALIDATION_LAYERS_ENABLED
    createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
    createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
#elif
    createInfo.enabledLayerCount = 0;
#endif


    if (vkCreateDevice(
            device->VulkanPhysicalDevice, &createInfo, nullptr, &device->VulkanDevice)
        != VK_SUCCESS) {
        yolo::error("failed to create logical device!");
        exit(1);
    }

    vkGetDeviceQueue(device->VulkanDevice, indices.graphicsFamily.value(), 0,
        &device->VulkanGraphicsQueue);
    vkGetDeviceQueue(device->VulkanDevice, indices.presentFamily.value(), 0,
        &device->VulkanPresentQueue);
}

void device_create_command_pool(GraphicsDevice* device)
{
    QueueFamilyIndices queueFamilyIndices
        = device_get_queue_families(device, device->VulkanPhysicalDevice);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
        | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(
            device->VulkanDevice, &poolInfo, nullptr, &device->VulkanCommandPool)
        != VK_SUCCESS) {
        yolo::error("failed to create command pool!");
    }
    yolo::info("Vulkan command pool created");
}

void device_resize_callback(GLFWwindow* window, int width, int height)
{
    auto device = (GraphicsDevice*)glfwGetWindowUserPointer(window);
    device->Resized = true;
    device->SurfaceSize = glm::ivec2(width, height);
    yolo::info("Window resized to {}x{}", width, height);
}

uint32_t device_find_memory_type(
    GraphicsDevice* g, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(g->VulkanPhysicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i))
            && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    yolo::error("Failed to find suitable memory type!");
    exit(1);
}

}
