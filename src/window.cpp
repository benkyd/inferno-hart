#include "window.hpp"

#include "gui/style.hpp"

#include <graphics.hpp>
#include <version.hpp>

#include "yolo/yolo.hpp"

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

void setupGLFW(std::string title)
{
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    uint32_t ext = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &ext, nullptr);
    yolo::info("Vulkan {} extensions supported", ext);

    // Create window with graphics context
    Window = glfwCreateWindow(1280, 720, title.c_str(), NULL, NULL);
    if (Window == NULL)
        throw std::runtime_error("Could not create window");

    if (!glfwVulkanSupported()) {
        yolo::error("Vulkan not supported");
        exit(1);
    }

    // Vulkan Init
    VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = INFERNO_VERSION;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Inferno";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instInfo {};
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtCount = 0;
    const char** exts = glfwGetRequiredInstanceExtensions(&ext);
    // add VK_KHR_xcb_surface to the list of extensions
    // TODO: This is a massive fuck off hack - SOLVE IT!!!!
    exts[glfwExtCount++] = "VK_KHR_xcb_surface";
    instInfo.enabledExtensionCount = glfwExtCount;
    instInfo.ppEnabledExtensionNames = exts;
    instInfo.enabledLayerCount = 0;
    yolo::info("GLFW requested {} extensions: {}", glfwExtCount, exts[0]);

    if (vkCreateInstance(&instInfo, nullptr, &VulkanInstance) != VK_SUCCESS) {
        yolo::error("Could not create Vulkan instance");
        exit(1);
    }

    // Physical Device
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(VulkanInstance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        yolo::error("No Vulkan devices found");
        exit(1);
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(VulkanInstance, &deviceCount, devices.data());

    // TODO: We need to do device suitability in a much better way
    const std::vector<const char*> deviceExtensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    for (const auto& device : devices) {
        const auto& checkDevExtensions = [&]() -> bool {
            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

            std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

            for (const auto& extension : availableExtensions) {
                requiredExtensions.erase(extension.extensionName);
            }

            return requiredExtensions.empty();
        };

        const auto& isDeviceSuitable
            = [&]() -> bool {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            VkPhysicalDeviceFeatures deviceFeatures;
            vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
            yolo::info("Found Vulkan device: {}", deviceProperties.deviceName);

            bool features = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;

            QueueFamilyIndices indices = window_get_queue_families(device);
            bool extensions = checkDevExtensions();

            return indices.isComplete() && extensions && features;
        };

        if (isDeviceSuitable()) {
            VulkanPhysicalDevice = device;
            break;
        }
    }

    if (VulkanPhysicalDevice == VK_NULL_HANDLE) {
        yolo::error("No discrete Vulkan devices found");
        exit(1);
    }

    // Logical Device
    QueueFamilyIndices indices = window_get_queue_families(VulkanPhysicalDevice);
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures {};
    VkDeviceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;
    createInfo.enabledLayerCount = 0;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (vkCreateDevice(VulkanPhysicalDevice, &createInfo, nullptr, &VulkanDevice) != VK_SUCCESS) {
        yolo::error("Could not create Vulkan logical device");
        exit(1);
    }

    vkGetDeviceQueue(VulkanDevice, indices.graphicsFamily.value(), 0, &VulkanPresentQueue);

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(VulkanPhysicalDevice, &deviceProperties);
    yolo::info("Vulkan running on ", deviceProperties.deviceName);

    // "Surface" creation
    VkResult result = glfwCreateWindowSurface(VulkanInstance, Window, nullptr, &VulkanSurface);
    if (result != VK_SUCCESS) {
        yolo::error("Could not create Vulkan surface (code: {})", result);
        exit(1);
    }

    // Swapchainnnnn
    // is this device suitable for a swapchain?
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    const auto& querySwapChainSupport
        = [](VkPhysicalDevice device) -> SwapChainSupportDetails {
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
    };

    const auto& chooseSwapSurfaceFormat
        = [](const std::vector<VkSurfaceFormatKHR>& availableFormats) -> VkSurfaceFormatKHR {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
                && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    };

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
    setupGLFW(title);
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

// VULKAN SPECIFIC
QueueFamilyIndices window_get_queue_families(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, VulkanSurface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}
// END VULKAN SPECIFIC

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
