#include "window.hpp"

#include "gui/style.hpp"

#include <graphics.hpp>
#include <version.hpp>

#include "yolo/yolo.hpp"

#include <map>
#include <optional>
#include <set>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace inferno::graphics {

static WINDOW_MODE WinMode = WINDOW_MODE::WIN_MODE_DEFAULT;
static KeyCallback UserKeyCallback = nullptr;
static int Width, Height;
static GLFWwindow* Window;

void glfwKeyCallback(GLFWwindow* window, int key, int scancode,
    int action, int mods)
{
    if (UserKeyCallback != nullptr) {
        UserKeyCallback(key, scancode, action, mods);
    }
}

void glfwErrorCallback(int error, const char* description)
{
    yolo::error("[GLFW {}] {}", error, description);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    yolo::warn("[VULKAN] {}", pCallbackData->pMessage);

    return VK_FALSE;
}

VkResult createDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
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
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
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

    std::vector<const char*> extensions(glfwExtensions,
        glfwExtensions + glfwExtensionCount);

    if constexpr (VALIDATION_LAYERS_ENABLED) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}




void setupWindow(std::string title)
{
    glfwInit();

    // tell glfw to not use a specific api? TODO: reasearch
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // disable resizing the window, this project is not configured to handle resizing
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    Window = glfwCreateWindow(Width, Height, title.c_str(), nullptr, nullptr);
}

void setupVulkan()
{
    window_create_vulkan_instance();
    window_vulkan_debugger();
    window_create_vulkan_surface();
    window_create_vulkan_physical_device();
    window_create_vulkan_logical_device();
}

void setupImGui()
{
    // // 1: create descriptor pool for IMGUI
    // //  the size of the pool is very oversize, but it's copied from imgui demo itself.
    // VkDescriptorPoolSize pool_sizes[] = {
    //     { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
    //     { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
    //     { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
    //     { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
    //     { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
    //     { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
    //     { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
    //     { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
    //     { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
    //     { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
    //     { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    // };
    //
    // VkDescriptorPoolCreateInfo pool_info = {};
    // pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    // pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    // pool_info.maxSets = 1000;
    // pool_info.poolSizeCount = std::size(pool_sizes);
    // pool_info.pPoolSizes = pool_sizes;
    //
    // VkDescriptorPool imguiPool;
    // VK_CHECK(vkCreateDescriptorPool(_device, &pool_info, nullptr, &imguiPool));
    // // 2: initialize imgui library
    //
    // // this initializes the core structures of imgui
    // ImGui::CreateContext();
    //
    // // this initializes imgui for SDL
    // ImGui_ImplSDL2_InitForVulkan(_window);
    //
    // // this initializes imgui for Vulkan
    // ImGui_ImplVulkan_InitInfo init_info = {};
    // init_info.Instance = _instance;
    // init_info.PhysicalDevice = _chosenGPU;
    // init_info.Device = _device;
    // init_info.Queue = _graphicsQueue;
    // init_info.DescriptorPool = imguiPool;
    // init_info.MinImageCount = 3;
    // init_info.ImageCount = 3;
    // init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    //
    // ImGui_ImplVulkan_Init(&init_info, _renderPass);
    //
    // // execute a gpu command to upload imgui font textures
    // immediate_submit([&](VkCommandBuffer cmd) {
    //     ImGui_ImplVulkan_CreateFontsTexture(cmd);
    // });
    //
    // // clear font textures from cpu data
    // ImGui_ImplVulkan_DestroyFontUploadObjects();
    //
    // // add the destroy the imgui created structures
    // _mainDeletionQueue.push_function([=]() {
    //     vkDestroyDescriptorPool(_device, imguiPool, nullptr);
    //     ImGui_ImplVulkan_Shutdown();
    // });
}

void shutdownGLFW()
{
    glfwDestroyWindow(Window);
    glfwTerminate();
}

void window_create(std::string title, int width, int height)
{
    Width = width;
    Height = height;
    setupWindow(title);
    setupVulkan();
    glfwSetKeyCallback(Window, glfwKeyCallback);
    // setupImGui();
}

void window_cleanup()
{
    vkDestroySurfaceKHR(VulkanInstance, VulkanSurface, nullptr);
    vkDestroyInstance(VulkanInstance, nullptr);
    vkDestroyDevice(VulkanDevice, nullptr);
    shutdownGLFW();
}

void window_create_vulkan_instance()
{
    if (VALIDATION_LAYERS_ENABLED && !checkValidationLayerSupport()) {
        throw std::runtime_error(
            "validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Inferno HART";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

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

    if (vkCreateInstance(&createInfo, nullptr, &VulkanInstance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
    yolo::info("Vulkan instance created");
}

void window_vulkan_debugger()
{
    if constexpr (!VALIDATION_LAYERS_ENABLED) {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (createDebugUtilsMessengerEXT(VulkanInstance, &createInfo, nullptr,
            &VulkanDebugMessenger)
        != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

void window_create_vulkan_surface()
{
    if (glfwCreateWindowSurface(VulkanInstance, Window, nullptr, &VulkanSurface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    yolo::info("Vulkan surface created");
}

void window_create_vulkan_physical_device()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(VulkanInstance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(VulkanInstance, &deviceCount, devices.data());

    // std::multimap is sorted list of key-value pairs. Sorted by key.
    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto& device : devices) {
        int score = window_evaluate_device(device);
        candidates.insert(std::make_pair(score, device));
    }

    // as candidates is a sorted list, the last value will always be the biggest score (first element)
    if (candidates.rbegin()->first > 0) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(candidates.rbegin()->second, &deviceProperties);
        yolo::info("Using device: {}", deviceProperties.deviceName);

        // established the device with the best score, or the only one in the system.
        VulkanPhysicalDevice = candidates.rbegin()->second;
    } else {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void window_create_vulkan_logical_device()
{
    QueueFamilyIndices indices = window_get_queue_families(VulkanPhysicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

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

    createInfo.enabledExtensionCount = 0;

#ifdef VALIDATION_LAYERS_ENABLED
    createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
    createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
#elif
    createInfo.enabledLayerCount = 0;
#endif

    if (vkCreateDevice(VulkanPhysicalDevice, &createInfo, nullptr, &VulkanDevice) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(VulkanDevice, indices.graphicsFamily.value(), 0, &VulkanGraphicsQueue);
    vkGetDeviceQueue(VulkanDevice, indices.presentFamily.value(), 0, &VulkanPresentQueue);
}

bool window_evaluate_device(VkPhysicalDevice device)
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
    // We really want to favour ones with RayTracing support

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    // Application won't function without geometry shaders
    if (!deviceFeatures.geometryShader)
        return 0;

    // Ensure that the device can process the graphics commands that we need
    QueueFamilyIndices indices = window_get_queue_families(device);
    if (!indices.isComplete())
        return 0;

    return score;
}

bool window_evaluate_device_extensions(VkPhysicalDevice device, std::vector<const char*> extensions)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

QueueFamilyIndices window_get_queue_families(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    uint32_t queueFamIndex = 0;

    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = queueFamIndex;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, queueFamIndex, VulkanSurface, &presentSupport);

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

SwapChainSupportDetails window_get_swap_chain_support(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, VulkanSurface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, VulkanSurface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, VulkanSurface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, VulkanSurface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, VulkanSurface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

void window_set_title(std::string title)
{
    glfwSetWindowTitle(Window, title.c_str());
}

void window_set_size(int w, int h)
{
    Width = w;
    Height = h;
    glfwSetWindowSize(Window, Width, Height);
}

void window_set_pos(int x, int y) { glfwSetWindowPos(Window, x, y); }

glm::vec2 window_get_size() { return { Width, Height }; }

void window_get_pos(int& x, int& y) { glfwGetWindowPos(Window, &x, &y); }

GLFWwindow* window_get_glfw_window() { return Window; }

void window_set_mode(WINDOW_MODE mode)
{
    WinMode = mode;
    if (mode == WINDOW_MODE::WIN_MODE_FPS) {
        glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
}

void window_set_key_callback(KeyCallback callback) { UserKeyCallback = callback; }

KeyCallback window_get_key_callback() { return UserKeyCallback; }

bool window_new_frame()
{
    glfwPollEvents();
    if (WinMode == WIN_MODE_FPS) {
        glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        glfwSetCursorPos(Window, (double)Width / 2, (double)Height / 2);
    }
    if (glfwWindowShouldClose(Window)) {
        return false;
    }

    glfwGetWindowSize(Window, &Width, &Height);

    // ImGui_ImplVulkan_NewFrame();
    // ImGui_ImplGlfw_NewFrame();
    // ImGui::NewFrame();
    //
    // ImGui::Begin("main", nullptr, WINDOW_FLAGS);
    // ImGui::SetWindowPos(ImVec2(0, 0));
    // ImGui::SetWindowSize(ImVec2(Width, Height));

    return true;
}

void window_render()
{
    // ImGui::End();
    // ImGui::Render();
    // auto io = ImGui::GetIO();
    // ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData());
    // glfwSwapBuffers(Window);
    // ImGui::UpdatePlatformWindows();
}
}
