#include "inferno.hpp"

#include <version.hpp>
//#include "gui/layout.hpp"
#include "window.hpp"

//#include "hart_module.hpp"
//#include "hart_directory.hpp"

//#include "preview_renderer/renderer.hpp"
//#include "preview_renderer/shader.hpp"
//#include "renderer/dispatcher.hpp"
//#include "renderer/renderer.hpp"
//#include "scene/camera.hpp"
//#include "scene/scene.hpp"
//#include "scene/material.hpp"
//#include "scene/mesh.hpp"

#include <yolo/yolo.hpp>

#include <iostream>
#include <memory>
#include <chrono>
#include <numeric>

namespace inferno::core {

InfernoApp* create_inferno()
{
    // MOTD
    yolo::info("INFERNO HART v" INFERNO_VERSION);

    InfernoApp* app = new InfernoApp();

    // Create window
    app->Win = &Window::GetInstance();
    app->Win->init("Inferno v" INFERNO_VERSION, 1280, 720);
    return app;
}

void cleanup_inferno(InfernoApp* app)
{
}

static void gui_help_marker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void preset_gui(InfernoApp *app)
{
    ImGuiID dockspace_id = ImGui::GetID("main");

    ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
    ImGui::DockBuilderSetNodeSize(dockspace_id, {1000, 1000});

    ImGuiID dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
    ImGuiID dock_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.5f, NULL, &dock_main_id);
    ImGui::DockBuilderDockWindow("Preview", dock_left);
    ImGui::DockBuilderDockWindow("Render", dock_main_id);
    ImGui::DockBuilderFinish(dockspace_id);

    yolo::info("LAYOUT SET TO DEFAULT");
}

void move_input(InfernoApp *app)
{
    static GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    glfwSetCursor(app->Win->getGLFWWindow(), cursor);

    // KBD & MOUSE
    // pan only get on hold
    static glm::dvec2 lastMousePos;
    static int firstClick = 0;
    if (glfwGetMouseButton(app->Win->getGLFWWindow(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
        firstClick++;
        if (firstClick == 1)
        {
            glfwGetCursorPos(app->Win->getGLFWWindow(), &lastMousePos.x, &lastMousePos.y);
        }
        glm::dvec2 tempMousePos = { 0.0f, 0.0f };
        glfwGetCursorPos(app->Win->getGLFWWindow(), &tempMousePos.x, &tempMousePos.y);
        app->Input->MouseDelta = lastMousePos - tempMousePos;
        lastMousePos = tempMousePos;
    } else
    {
        firstClick = 0;
        app->Input->MouseDelta = { 0.0f, 0.0f };
        lastMousePos = { 0.0f, 0.0f };
    }

    app->Input->MovementDelta = 0b00000000;
    if (glfwGetKey(app->Win->getGLFWWindow(), GLFW_KEY_W) == GLFW_PRESS)
        app->Input->MovementDelta |= 0b10000000;
    if (glfwGetKey(app->Win->getGLFWWindow(), GLFW_KEY_A) == GLFW_PRESS)
        app->Input->MovementDelta |= 0b01000000;
    if (glfwGetKey(app->Win->getGLFWWindow(), GLFW_KEY_S) == GLFW_PRESS)
        app->Input->MovementDelta |= 0b00100000;
    if (glfwGetKey(app->Win->getGLFWWindow(), GLFW_KEY_D) == GLFW_PRESS)
        app->Input->MovementDelta |= 0b00010000;
    if (glfwGetKey(app->Win->getGLFWWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
        app->Input->MovementDelta |= 0b00001000;
    if (glfwGetKey(app->Win->getGLFWWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        app->Input->MovementDelta |= 0b00000100;
}

void stop_move_input(InfernoApp *app)
{
    app->Input->MovementDelta = 0x0;
    app->Input->MouseDelta = { 0.0f, 0.0f };
}

int run(InfernoApp *app)
{
    return 1;
}

}

