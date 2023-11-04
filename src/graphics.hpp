#pragma once

// easy include for graphics shit

// Include GLFW and ImGUI
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

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
