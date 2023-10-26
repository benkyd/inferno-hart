#pragma once

// easy include for graphics shit

// Include OpenGL
extern "C" 
{
#include <glad/glad.h>
}

// Include GLFW and ImGUI
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
namespace inferno {
namespace graphics::rays {
class Ray;
}
using RayField = std::vector<graphics::rays::Ray*>;
}
