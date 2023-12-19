#pragma once

#include "graphics.hpp"
#include "imgui/imgui.h"
#include "style.hpp"

#include "graphics/device.hpp"
#include "graphics/swapchain.hpp"
#include "graphics/vkrenderer.hpp"

#include "window.hpp"

#include "yolo/yolo.hpp"

namespace inferno::gui {

#define WINDOW_FLAGS                                                                     \
    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse                            \
        | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove                            \
        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus           \
        | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground                       \
        | ImGuiWindowFlags_NoDecoration | ImGuiMouseCursor_Arrow

inline void imgui_init(graphics::VulkanRenderer* renderer)
{
    graphics::GraphicsDevice* device = renderer->Device;
    VkDescriptorPoolSize pool_sizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    VkDescriptorPool imguiPool;
    vkCreateDescriptorPool(device->VulkanDevice, &pool_info, nullptr, &imguiPool);

    // this initializes the core structures of imgui
    ImGui::CreateContext();

    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplGlfw_InitForVulkan(graphics::window_get_glfw_window(), true);

    // this initializes imgui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = device->VulkanInstance;
    init_info.PhysicalDevice = device->VulkanPhysicalDevice;
    init_info.Device = device->VulkanDevice;
    init_info.Queue = device->VulkanGraphicsQueue;
    init_info.DescriptorPool = imguiPool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.UseDynamicRendering = true;
    init_info.ColorAttachmentFormat = renderer->Swap->ImageFormat;
    ImGui_ImplVulkan_Init(&init_info, VK_NULL_HANDLE);

    SetupImGuiStyle2();

    yolo::info("Initialized ImGUI");
}

inline void imgui_new_frame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("main", nullptr, WINDOW_FLAGS);

    ImGui::SetWindowPos({ 0, 0 });
    ImGui::SetWindowSize(
    { graphics::window_get_size().x, graphics::window_get_size().y });

    ImGuiID dockspace_id = ImGui::GetID("main");
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
}

inline void imgui_render_frame(VkCommandBuffer command_buffer)
{
    ImGui::End();

    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(draw_data, command_buffer);
}

inline void imgui_shutdown()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

}

