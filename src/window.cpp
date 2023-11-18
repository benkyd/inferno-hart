#include "window.hpp"

#include "gui/style.hpp"
#include "graphics/device.hpp"

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

void setupWindow(std::string title)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    Window = glfwCreateWindow(Width, Height, title.c_str(), nullptr, nullptr);
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
    glfwSetKeyCallback(Window, glfwKeyCallback);
    // setupImGui();
}

void window_set_surface(GraphicsDevice* device)
{
     if (glfwCreateWindowSurface(device->VulkanInstance, Window, nullptr, &device->VulkanSurface) != VK_SUCCESS) {
         yolo::error("failed to create window surface!");
         exit(1);
    }
    yolo::info("Vulkan surface created");
}

void window_cleanup()
{
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
