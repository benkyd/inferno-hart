#include "inferno.hpp"

#include <graphics.hpp>
#include <version.hpp>

// #include "gui/layout.hpp"
// #include "renderer/renderer.hpp"
// #include "scene/scene.hpp"
#include "graphics/buffer.hpp"
#include "graphics/device.hpp"
#include "graphics/pipeline.hpp"
#include "graphics/shader.hpp"
#include "graphics/swapchain.hpp"
#include "graphics/vkrenderer.hpp"
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

    app->Camera = graphics::camera_create();
    graphics::camera_set_position(app->Camera, { 0.0f, 1.0f, 3.1f });

    // Create window
    graphics::window_create("Inferno v" INFERNO_VERSION, 1920, 1080);
    app->Device = graphics::device_create();
    app->Renderer = graphics::renderer_create(app->Device);
    graphics::renderer_configure_command_buffer(app->Renderer);

    app->Shader = graphics::shader_create(app->Device, app->Renderer->Swap);
    graphics::shader_load(app->Shader, "res/shaders/basic");
    graphics::shader_build(app->Shader);

    // std::vector<scene::Vert> verticies
    //     = { { { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
    //           { { 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
    //           { { 0.5f, 0.5, 0.0f }, { 0.0f, 0.0f, 1.0f } },
    //           { { -0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f } } };
    //
    // app->VBuffer
    //     = graphics::vertex_buffer_create(app->Device, verticies.data(),
    //     verticies.size());
    //
    // std::vector<scene::Index> indicies = { 0, 1, 2, 2, 3, 0 };
    //
    // app->IBuffer
    //     = graphics::index_buffer_create(app->Device, indicies.data(), indicies.size());

    // setup the scene
    // scene::Material* basicMaterial = new scene::Material("basic");
    // graphics::Shader* basicShader = graphics::shader_create();
    // graphics::shader_load(basicShader, "res/shaders/basic.glsl");
    // graphics::shader_link(basicShader);
    // basicMaterial->setGlShader(basicShader);

    scene::Mesh* mesh = scene::mesh_create(app->Device);
    scene::mesh_load_obj(mesh, "res/cornell-box.obj");
    scene::mesh_ready(mesh);
    // mesh->setMaterial(basicMaterial);
    scene::SceneObject* object = scene::scene_object_create();
    scene::scene_object_add_mesh(object, mesh);
    scene::scene_add_object(app->Scene, object);

    scene::Mesh* lucy = scene::mesh_create(app->Device);
    scene::mesh_load_obj(lucy, "res/lucy.obj");
    scene::mesh_ready(lucy);
    // mesh->setMaterial(basicMaterial);
    scene::SceneObject* lucyObject = scene::scene_object_create();
    scene::scene_object_add_mesh(lucyObject, lucy);
    scene::scene_add_object(app->Scene, lucyObject);

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
    // ImGui::TextDisabled("(?)");
    // if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
    //     ImGui::BeginTooltip();
    //     ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    //     ImGui::TextUnformatted(desc);
    //     ImGui::PopTextWrapPos();
    //     ImGui::EndTooltip();
    // }
}

void inferno_preset_gui(InfernoApp* app)
{
    // ImGuiID dockspace_id = ImGui::GetID("main");
    //
    // ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
    // ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty
    // node ImGui::DockBuilderSetNodeSize(dockspace_id, { 1000, 1000 });
    //
    // ImGuiID dock_main_id = dockspace_id;
    // ImGuiID dock_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.5f,
    // NULL, &dock_main_id); ImGui::DockBuilderDockWindow("Preview", dock_left);
    // ImGui::DockBuilderDockWindow("Render", dock_main_id);
    // ImGui::DockBuilderFinish(dockspace_id);
    //
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
    app->FrameCount++;
    if (app->FrameCount % 100 == 0) {
        yolo::info(
            "Average FPS: {}", 1.0 / inferno_timer_get_time(app->MainTimer).count());
        inferno_timer_print(app->MainTimer, false);
    }

    if (!graphics::window_new_frame())
        return false;

    // set the main window to the dockspace and then on the first launch set the preset
    // ImGuiID dockspace_id = ImGui::GetID("main");
    // static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    // if (ImGui::DockBuilderGetNode(dockspace_id) == NULL) {
    //     inferno_preset_gui(app);
    // }
    // ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    //
    return true;
}

void inferno_end(InfernoApp* app) { }

int inferno_run(InfernoApp* app)
{
    while (true) {
        inferno_timer_start(app->MainTimer);
        if (!inferno_pre(app))
            break;

        if (!graphics::renderer_begin_frame(app->Renderer))
            continue;

        VkCommandBuffer commandBuffer
            = app->Renderer->CommandBuffersInFlight[app->Renderer->CurrentFrameIndex];
        uint32_t frameIndex = app->Renderer->CurrentFrameIndex;

        // shader bind
        graphics::shader_use(app->Shader, commandBuffer);
        scene::GlobalUniformObject globalUniformObject {
            .Projection = graphics::camera_get_projection(app->Camera),
            .View = graphics::camera_get_view(app->Camera),
        };

        graphics::shader_update_state(
            app->Shader, commandBuffer, globalUniformObject, frameIndex);

        for (auto& object : scene::scene_get_renderables(app->Scene)) {
            graphics::vertex_buffer_bind(object->Meshs[0]->VertexBuffer, commandBuffer);
            graphics::index_buffer_bind(object->Meshs[0]->IndexBuffer, commandBuffer);

            vkCmdDrawIndexed(commandBuffer,
                object->Meshs[0]->IndexBuffer->GenericBuffer->Count, 1, 0, 0, 0);
        }

        // graphics::shader_update_state(
        //     app->Shader, commandBuffer, globalUniformObject, frameIndex);
        //
        // // Put these two in the bloody mesh code
        // graphics::vertex_buffer_bind(app->VBuffer, commandBuffer);
        // graphics::index_buffer_bind(app->IBuffer, commandBuffer);

        // vkCmdDrawIndexed(commandBuffer, app->IBuffer->GenericBuffer->Count, 1, 0, 0,
        // 0);

        graphics::renderer_draw_frame(app->Renderer);

        if (glm::length(app->Input->MouseDelta) > 0.0f)
            graphics::camera_mouse_move(app->Camera, app->Input->MouseDelta,
                inferno_timer_get_time(app->MainTimer).count());
        if (app->Input->MovementDelta != 0b00000000)
            graphics::camera_move(app->Camera, app->Input->MovementDelta,
                inferno_timer_get_time(app->MainTimer).count());

        inferno_move_input(app, inferno_timer_get_time(app->MainTimer));

        //     // Menu Bar
        //     static bool showPreview = true;
        //     static bool showRenderSettings = true;
        //     static bool showDemoWindow = false;
        //     if (ImGui::BeginMenuBar()) {
        //         if (ImGui::BeginMenu("Menu")) {
        //             ImGui::EndMenu();
        //         }
        //         if (ImGui::BeginMenu("View")) {
        //             ImGui::Checkbox("Show Preview", &showPreview);
        //             ImGui::SameLine();
        //             inferno_gui_help_marker("Show the preview window");
        //             ImGui::Checkbox("Show Settings", &showRenderSettings);
        //             ImGui::SameLine();
        //             inferno_gui_help_marker("Show the Inferno HART settings window");
        //             ImGui::Checkbox("Show Demo", &showDemoWindow);
        //
        //             ImGui::EndMenu();
        //         }
        //         ImGui::EndMenuBar();
        //     }
        //
        //     if (showRenderSettings && ImGui::Begin("Inferno HART")) {
        //         if (ImGui::TreeNode("Camera")) {
        //             graphics::Camera* camera = scene::scene_get_camera(app->Scene);
        //             graphics::camera_draw_ui(camera);
        //             ImGui::TreePop();
        //         }
        //         if (ImGui::TreeNode("Preview Render")) {
        //             ImGui::Checkbox("Show Preview", &showPreview);
        //             graphics::preview_draw_ui(app->PreviewRenderer);
        //             if (ImGui::TreeNode("Debug Overlay")) {
        //                 graphics::debug_draw_ui();
        //                 ImGui::TreePop();
        //             }
        //             ImGui::TreePop();
        //         }
        //         if (ImGui::TreeNode("RayTraced Render")) {
        //             graphics::rayr_draw_ui(app->RayRenderer);
        //             ImGui::TreePop();
        //         }
        //         ImGui::End();
        //     }
        //
        //     if (showPreview && ImGui::Begin("Preview", nullptr,
        //     ImGuiWindowFlags_NoScrollbar)) {
        //         if (ImGui::IsWindowHovered()) {
        //             inferno_move_input(app, inferno_timer_get_time(app->MainTimer));
        //         } else {
        //             inferno_stop_move_input(app);
        //         }
        //
        //         graphics::camera_raster_set_viewport(scene::scene_get_camera(app->Scene),
        //             { ImGui::GetWindowSize().x, ImGui::GetWindowSize().y });
        //         graphics::preview_set_viewport(app->PreviewRenderer,
        //         app->Scene->Camera);
        //
        //         graphics::preview_draw(app->PreviewRenderer, app->Scene);
        //         graphics::debug_draw_to_target(app->Scene);
        //
        //         ImTextureID texture =
        //         (ImTextureID)graphics::preview_get_rendered_texture(app->PreviewRenderer);
        //         ImGui::Image(
        //             texture,
        //             { ImGui::GetWindowSize().x, ImGui::GetWindowSize().y },
        //             ImVec2(0, 1), ImVec2(1, 0));
        //
        //         ImGui::End();
        //     }
        //
        //     if (ImGui::Begin("Render")) {
        //         graphics::rayr_draw(app->RayRenderer);
        //
        //         ImTextureID texture =
        //         (ImTextureID)graphics::rayr_get_rendered_texture(app->RayRenderer);
        //         ImGui::Image(
        //             texture,
        //             { ImGui::GetWindowSize().x, ImGui::GetWindowSize().y },
        //             ImVec2(0, 1), ImVec2(1, 0));
        //         ImGui::End();
        //     }
        //
        //     if (showDemoWindow) {
        //         ImGui::ShowDemoWindow();
        //     }
        //
        graphics::window_render();
        inferno_end(app);
        inferno_timer_end(app->MainTimer);
    }
    //
    return 1;
}

} // namespace inferno
