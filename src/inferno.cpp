#include "inferno.hpp"

#include <version.hpp>
// #include "gui/layout.hpp"
#include "imgui/imgui.h"
#include "scene/scene.hpp"
#include "window.hpp"

#include "preview_renderer/renderer.hpp"
#include "preview_renderer/shader.hpp"
#include "scene/scene.hpp"
#include "scene/camera.hpp"
#include "scene/material.hpp"
#include "scene/mesh.hpp"

#include <yolo/yolo.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <numeric>

namespace inferno {

InfernoApp* inferno_create()
{
    // MOTD
    yolo::info("INFERNO HART v" INFERNO_VERSION);

    InfernoApp* app = new InfernoApp;
    app->Input = new InfernoInput;

    // Create window
    graphics::window_create("Inferno v" INFERNO_VERSION, 1280, 720);

    // setup the scene
    scene::Material basicMaterial("basic");
    graphics::Shader* basicShader = graphics::shader_create();
    graphics::shader_load(basicShader, "res/shaders/basic.glsl");
    graphics::shader_link(basicShader);

    app->Scene = scene::scene_create();
    scene::SceneObject* object = scene::scene_object_create();

    scene::Mesh cornell;
    cornell.loadOBJ("res/cornell-box.obj");
    // cornell.loadOBJ("res/sponza.obj");
    cornell.ready();
    cornell.setMaterial(&basicMaterial);
    scene::scene_object_add_mesh(object, &cornell);

    yolo::info("{} {}", app->Scene, object);
    scene::scene_add_object(app->Scene, object);

    return app;
}

void inferno_cleanup(InfernoApp* app)
{
    graphics::window_cleanup();
    delete app;
}

static void inferno_gui_help_marker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void inferno_preset_gui(InfernoApp* app)
{
    ImGuiID dockspace_id = ImGui::GetID("main");

    ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
    ImGui::DockBuilderSetNodeSize(dockspace_id, { 1000, 1000 });

    ImGuiID dock_main_id = dockspace_id;
    ImGuiID dock_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.5f, NULL, &dock_main_id);
    ImGui::DockBuilderDockWindow("Preview", dock_left);
    ImGui::DockBuilderDockWindow("Render", dock_main_id);
    ImGui::DockBuilderFinish(dockspace_id);

    yolo::info("LAYOUT SET TO DEFAULT");
}

void inferno_move_input(InfernoApp* app)
{
    static GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    glfwSetCursor(graphics::window_get_glfw_window(), cursor);

    // KBD & MOUSE
    // pan only get on hold
    static glm::dvec2 lastMousePos;
    static int firstClick = 0;
    if (glfwGetMouseButton(graphics::window_get_glfw_window(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
        firstClick++;
        if (firstClick == 1) {
            glfwGetCursorPos(graphics::window_get_glfw_window(), &lastMousePos.x, &lastMousePos.y);
        }
        glm::dvec2 tempMousePos = { 0.0f, 0.0f };
        glfwGetCursorPos(graphics::window_get_glfw_window(), &tempMousePos.x, &tempMousePos.y);
        app->Input->MouseDelta = lastMousePos - tempMousePos;
        lastMousePos = tempMousePos;
    } else {
        firstClick = 0;
        app->Input->MouseDelta = { 0.0f, 0.0f };
        lastMousePos = { 0.0f, 0.0f };
    }

    app->Input->MovementDelta = 0b00000000;
    if (glfwGetKey(graphics::window_get_glfw_window(), GLFW_KEY_W) == GLFW_PRESS)
        app->Input->MovementDelta |= 0b10000000;
    if (glfwGetKey(graphics::window_get_glfw_window(), GLFW_KEY_A) == GLFW_PRESS)
        app->Input->MovementDelta |= 0b01000000;
    if (glfwGetKey(graphics::window_get_glfw_window(), GLFW_KEY_S) == GLFW_PRESS)
        app->Input->MovementDelta |= 0b00100000;
    if (glfwGetKey(graphics::window_get_glfw_window(), GLFW_KEY_D) == GLFW_PRESS)
        app->Input->MovementDelta |= 0b00010000;
    if (glfwGetKey(graphics::window_get_glfw_window(), GLFW_KEY_SPACE) == GLFW_PRESS)
        app->Input->MovementDelta |= 0b00001000;
    if (glfwGetKey(graphics::window_get_glfw_window(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        app->Input->MovementDelta |= 0b00000100;
}

void inferno_stop_move_input(InfernoApp* app)
{
    app->Input->MovementDelta = 0x0;
    app->Input->MouseDelta = { 0.0f, 0.0f };
}

int inferno_run(InfernoApp* app)
{
    while (true) {
        if (!graphics::window_new_frame())
            break;

        // // set the main window to the dockspace and then on the first launch set the preset
        // ImGuiID dockspace_id = ImGui::GetID("main");
        // static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        // if (ImGui::DockBuilderGetNode(dockspace_id) == NULL) {
        //     inferno_preset_gui(app);
        // }
        // ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        //
        // yolo::debug("{} {} {}", app->Scene->Camera, app->Input, app->Input->MouseDelta.x);
        // if (glm::length(app->Input->MouseDelta) > 0.0f)
        //     graphics::camera_mouse_move(app->Scene->Camera, app->Input->MouseDelta);
        // if (app->Input->MovementDelta != 0b00000000)
        //     graphics::camera_move(app->Scene->Camera, app->Input->MovementDelta);

        // // Menu Bar
        // static bool showPreview = true;
        // static bool showRenderSettings = true;
        // static bool showDemoWindow = false;
        // if (ImGui::BeginMenuBar()) {
        //     if (ImGui::BeginMenu("Menu")) {
        //         ImGui::EndMenu();
        //     }
        //     if (ImGui::BeginMenu("View")) {
        //         ImGui::Checkbox("Show Preview", &showPreview);
        //         ImGui::SameLine();
        //         inferno_gui_help_marker("Show the preview window");
        //         ImGui::Checkbox("Show Settings", &showRenderSettings);
        //         ImGui::SameLine();
        //         inferno_gui_help_marker("Show the Inferno HART settings window");
        //         ImGui::Checkbox("Show Demo", &showDemoWindow);
        //
        //         ImGui::EndMenu();
        //     }
        //     ImGui::EndMenuBar();
        // }
        //
        // if (showPreview && ImGui::Begin("Preview", nullptr, ImGuiWindowFlags_NoScrollbar)) {
        //     if (ImGui::IsWindowHovered()) {
        //         inferno_move_input(app);
        //     } else {
        //         inferno_stop_move_input(app);
        //     }
        //
        //     graphics::raster_set_viewport(scene::scene_get_camera(app->Scene), { ImGui::GetWindowSize().x, ImGui::GetWindowSize().y });
        //     graphics::preview_draw(app->PreviewRenderer, app->Scene);
        //
        //     ImTextureID texture = (ImTextureID)graphics::preview_get_rendered_texture(app->PreviewRenderer);
        //     ImGui::Image(
        //         texture,
        //         { ImGui::GetWindowSize().x, ImGui::GetWindowSize().y },
        //         ImVec2(0, 1), ImVec2(1, 0));
        //
        //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //     ImGui::End();
        // }

        // clang-format off
        // GLenum err;
        // while((err = glGetError()) != GL_NO_ERROR) {
        //     std::string error;
        //     switch (err) {
        //         case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        //         case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        //         case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        //         case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        //         case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        //         case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        //         case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        //         default:                               error = std::to_string((uint32_t)err); break;
        //     }
        //     yolo::error("[GL]: {} {}", err, error);
        // }
        //
        graphics::window_render();
    }

    return 1;
}

} // namespace inferno
