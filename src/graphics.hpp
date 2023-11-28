#pragma once

// easy include for graphics shit
#define FRAMES_IN_FLIGHT 3
#define SHADER_STAGES 2

// Include GLFW and ImGUI
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32
#include <windows.h>
#else
#define VK_USE_PLATFORM_XLIB_KHR
#define GLFW_EXPOSE_NATIVE_X11
#include <X11/Xlib.h>
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_xlib.h>
#include <GLFW/glfw3native.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"
#include "imgui/imgui_internal.h"

// glm
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
namespace inferno {
namespace graphics::rays {
    class Ray;
}
using RayField = std::vector<graphics::rays::Ray*>;
}
