#include "inferno.hpp"

#include <cstdint>
#include <graphics.hpp>
#include <version.hpp>

#include "graphics/rendertarget.hpp"
#include "gui/gui.hpp"
// #include "renderer/renderer.hpp"
// #include "scene/scene.hpp"
#include "graphics/buffer.hpp"
#include "graphics/device.hpp"
#include "graphics/pipeline.hpp"
#include "graphics/shader.hpp"
#include "graphics/swapchain.hpp"
#include "graphics/vkrenderer.hpp"
#include "preview_renderer/debug.hpp"
#include "window.hpp"

// #include "preview_renderer/debug.hpp"
// #include "preview_renderer/renderer.hpp"
#include "scene/camera.hpp"
// #include "scene/material.hpp"
#include "scene/mesh.hpp"
#include "scene/scene.hpp"

#include <yolo/yolo.hpp>

#include <chrono>
#include <numeric>

namespace inferno {

InfernoTimer* inferno_timer_create()
{
    InfernoTimer* timer = new InfernoTimer;
    // lazy init
    inferno_timer_start(timer);
    inferno_timer_end(timer);
    return timer;
}

void inferno_timer_cleanup(InfernoTimer* timer) { delete timer; }

void inferno_timer_rolling_average(InfernoTimer* timer, int count)
{
    if (timer->Times.size() > count)
        timer->Times.erase(timer->Times.begin());
    timer->RollingAverage = std::accumulate(timer->Times.begin(), timer->Times.end(),
                                std::chrono::duration<double>(0.0))
        / count;
}

void inferno_timer_start(InfernoTimer* timer)
{
    timer->StartTime = std::chrono::high_resolution_clock::now();
}

void inferno_timer_end(InfernoTimer* timer)
{
    timer->EndTime = std::chrono::high_resolution_clock::now();
    timer->Time = timer->EndTime - timer->StartTime;
    timer->Times.push_back(timer->Time);
    inferno_timer_rolling_average(timer, 1000);
}

void inferno_timer_print(InfernoTimer* timer, bool time)
{
    if (time)
        yolo::info("Time: {}s", timer->Time.count());
    yolo::info("Average time: {}ms", timer->RollingAverage.count() * 1000.0);
}

std::chrono::duration<double> inferno_timer_get_time(InfernoTimer* timer)
{
    return timer->Time;
}

std::chrono::duration<double> inferno_timer_get_average(InfernoTimer* timer)
{
    return timer->RollingAverage;
}

InfernoApp* inferno_create()
{
    // MOTD
    yolo::info("INFERNO HART v" INFERNO_VERSION);

    InfernoApp* app = new InfernoApp;
    app->Input = new InfernoInput;
    app->Scene = scene::scene_create();
    app->MainTimer = inferno_timer_create();

    graphics::camera_set_position(app->Scene->Camera, { 0.0f, 1.0f, 3.1f });

    // Create window
    graphics::window_create("Inferno v" INFERNO_VERSION, 1920, 1080);
    app->Device = graphics::device_create();
    app->Renderer = graphics::renderer_create(app->Device);

    graphics::renderer_configure_gui(app->Renderer);
    graphics::renderer_configure_command_buffer(app->Renderer);

    app->PreviewRenderer = graphics::preview_create(app->Renderer);
    graphics::debug_do_depth_test(false);

    graphics::renderer_submit_repeat(
        app->Renderer,
        [](graphics::VulkanRenderer* renderer, VkCommandBuffer* commandBuffer) {
            gui::imgui_new_frame();
        },
        false);
    graphics::renderer_submit_repeat(
        app->Renderer,
        [&](graphics::VulkanRenderer* renderer, VkCommandBuffer* commandBuffer) {
            graphics::renderer_begin_pass(renderer,
                { 0, 0, (uint32_t)graphics::window_get_size().x,
                    (uint32_t)graphics::window_get_size().y });
            gui::imgui_render_frame(*commandBuffer);
            graphics::renderer_end_pass(renderer);
        },
        true);

    scene::SceneObject* object = scene::scene_object_create(app->Device);
    scene::scene_object_load(object, "res/Bistro.obj");
    // scene::scene_object_load(object, "res/sponza.obj");
    // scene::mesh_load_obj(object, "res/cornell-box.obj");
    scene::scene_add_object(app->Scene, object);

    // scene::Mesh* lucy = scene::mesh_create(app->Device);
    // scene::mesh_load_obj(lucy, "res/lucy.obj");
    // scene::mesh_ready(lucy);
    // scene::SceneObject* lucyObject = scene::scene_object_create();
    // scene::scene_object_add_mesh(lucyObject, lucy);
    // scene::scene_add_object(app->Scene, lucyObject);

    // app->PreviewRenderer = graphics::preview_create();
    // graphics::preview_set_viewport(app->PreviewRenderer, app->Scene->Camera);
    // app->RayRenderer = graphics::rayr_create(app->Scene);
    // graphics::rayr_set_viewport(app->RayRenderer, app->Scene->Camera);

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
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty
    // node ImGui::DockBuilderSetNodeSize(dockspace_id, { 1000, 1000 });

    ImGuiID dock_main_id = dockspace_id;
    ImGuiID dock_left = ImGui::DockBuilderSplitNode(
        dock_main_id, ImGuiDir_Left, 0.5f, NULL, &dock_main_id);
    ImGui::DockBuilderDockWindow("Preview", dock_left);
    ImGui::DockBuilderDockWindow("Render", dock_main_id);
    ImGui::DockBuilderFinish(dockspace_id);

    yolo::info("LAYOUT SET TO DEFAULT");
}

void inferno_move_input(InfernoApp* app, std::chrono::duration<double> deltaTime)
{
    static GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    glfwSetCursor(graphics::window_get_glfw_window(), cursor);

    // KBD & MOUSE
    // pan only get on hold
    static glm::dvec2 lastMousePos;
    static int firstClick = 0;
    if (glfwGetMouseButton(graphics::window_get_glfw_window(), GLFW_MOUSE_BUTTON_1)
        == GLFW_PRESS) {
        firstClick++;
        if (firstClick == 1) {
            glfwGetCursorPos(
                graphics::window_get_glfw_window(), &lastMousePos.x, &lastMousePos.y);
        }
        glm::dvec2 tempMousePos = { 0.0f, 0.0f };
        glfwGetCursorPos(
            graphics::window_get_glfw_window(), &tempMousePos.x, &tempMousePos.y);
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

bool inferno_pre(InfernoApp* app)
{
    inferno_timer_start(app->MainTimer);

    app->FrameCount++;
    if (app->FrameCount % 100 == 0) {
        yolo::info(
            "Average FPS: {}", 1.0 / inferno_timer_get_time(app->MainTimer).count());
        inferno_timer_print(app->MainTimer, false);
    }

    if (!graphics::renderer_begin_frame(app->Renderer))
        return false;

    return true;
}

void inferno_end(InfernoApp* app)
{
    graphics::renderer_draw_frame(app->Renderer);
    graphics::window_render();
    inferno_timer_end(app->MainTimer);
}

int inferno_run(InfernoApp* app)
{
    while (graphics::window_new_frame()) {
        if (!inferno_pre(app))
            continue;

        if (glm::length(app->Input->MouseDelta) > 0.0f)
            graphics::camera_mouse_move(app->Scene->Camera, app->Input->MouseDelta,
                inferno_timer_get_time(app->MainTimer).count());
        if (app->Input->MovementDelta != 0b00000000)
            graphics::camera_move(app->Scene->Camera, app->Input->MovementDelta,
                inferno_timer_get_time(app->MainTimer).count());

        // Menu Bar
        static bool showPreview = true;
        static bool showRenderSettings = true;
        static bool showDemoWindow = false;
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Menu")) {
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                ImGui::Checkbox("Show Preview", &showPreview);
                ImGui::SameLine();
                inferno_gui_help_marker("Show the preview window");
                ImGui::Checkbox("Show Settings", &showRenderSettings);
                ImGui::SameLine();
                inferno_gui_help_marker("Show the Inferno HART settings window");
                ImGui::Checkbox("Show Demo", &showDemoWindow);

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if (showRenderSettings && ImGui::Begin("Inferno HART")) {
            if (ImGui::TreeNode("Camera")) {
                graphics::camera_draw_ui(scene::scene_get_camera(app->Scene));
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Preview Render")) {
                ImGui::Checkbox("Show Preview", &showPreview);
                graphics::preview_draw_ui(app->PreviewRenderer);
                if (ImGui::TreeNode("Debug Overlay")) {
                    graphics::debug_draw_ui();
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("RayTraced Render")) {
                // graphics::rayr_draw_ui(app->RayRenderer);
                ImGui::TreePop();
            }
            ImGui::End();
        }

        if (showPreview
            && ImGui::Begin("Preview", nullptr, ImGuiWindowFlags_NoScrollbar)) {
            if (ImGui::IsWindowHovered()) {
                inferno_move_input(app, inferno_timer_get_time(app->MainTimer));
            } else {
                inferno_stop_move_input(app);
            }

            static ImVec2 lastViewport = { 0, 0 };
            ImVec2 currentViewport = ImGui::GetWindowSize();
            if (lastViewport.x != currentViewport.x
                || lastViewport.y != currentViewport.y) {
                graphics::camera_raster_set_viewport(scene::scene_get_camera(app->Scene),
                    { ImGui::GetWindowSize().x, ImGui::GetWindowSize().y });
                // if imgui has changed the viewport, we need to recreate the rendertarget
                graphics::preview_set_viewport(app->PreviewRenderer, app->Scene->Camera);
            }
            lastViewport = currentViewport;

            graphics::preview_draw(app->PreviewRenderer, app->Scene);
            graphics::debug_draw_line({ 0, 0, 0 }, { 1, 1, 0 }, { 1, 1, 0 });
            graphics::debug_draw_to_preview(app->Scene);

            ImTextureID texture
                = (ImTextureID)graphics::preview_get_target(app->PreviewRenderer)
                      ->DescriptorSet;
            ImGui::Image(texture, { ImGui::GetWindowSize().x, ImGui::GetWindowSize().y },
                ImVec2(0, 1), ImVec2(1, 0));
            ImGui::End();
        }

        if (ImGui::Begin("Render")) {
            // graphics::rayr_draw(app->RayRenderer);

            // ImTextureID texture
            //     = (ImTextureID)graphics::rayr_get_rendered_texture(app->RayRenderer);
            // ImGui::Image(texture, { ImGui::GetWindowSize().x, ImGui::GetWindowSize().y
            // },
            //     ImVec2(0, 1), ImVec2(1, 0));
            ImGui::End();
        }

        if (showDemoWindow) {
            ImGui::ShowDemoWindow();
        }

        inferno_end(app);
    }
    return 1;
}

} // namespace inferno
