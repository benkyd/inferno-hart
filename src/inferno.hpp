#pragma once

#include "graphics.hpp"
#include "graphics/device.hpp"
#include "scene/mesh.hpp"
#include "scene/scene.hpp"
// #include "scene/camera.hpp"
#include "preview_renderer/renderer.hpp"
#include "renderer/renderer.hpp"

#include <memory>

namespace inferno {

namespace graphics {
    struct Camera;
    struct GraphicsDevice;
    struct VulkanRenderer;
    struct Buffer;
    struct Shader;
}

namespace scene {
    struct Scene;
    struct Vert;
}

typedef struct InfernoTimer {
    std::chrono::time_point<std::chrono::high_resolution_clock> StartTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> EndTime;
    std::chrono::duration<double> Time;
    std::vector<std::chrono::duration<double>> Times;
    std::chrono::duration<double> RollingAverage;

} InfernoTimer;

InfernoTimer* inferno_timer_create();
void inferno_timer_cleanup(InfernoTimer* timer);

void inferno_timer_rolling_average(InfernoTimer* timer, int count);

void inferno_timer_start(InfernoTimer* timer);
void inferno_timer_end(InfernoTimer* timer);
void inferno_timer_print(InfernoTimer* timer, bool time);

std::chrono::duration<double> inferno_timer_get_time(InfernoTimer* timer);
std::chrono::duration<double> inferno_timer_get_average(InfernoTimer* timer);

typedef struct InfernoInput {
    glm::vec2 MouseDelta = { 0.0f, 0.0f };
    uint8_t MovementDelta = 0;
} InfernoInput;

typedef struct InfernoApp {
    InfernoInput* Input;
    scene::Scene* Scene;

    graphics::Buffer* VBuffer;
    graphics::Buffer* IBuffer;
    graphics::Shader* Shader;

    // graphics::PreviewRenderer* PreviewRenderer;
    graphics::RayRenderer* RayRenderer;

    graphics::VulkanRenderer* Renderer;
    graphics::GraphicsDevice* Device;

    InfernoTimer* MainTimer;
    uint64_t FrameCount;
} InfernoApp;

InfernoApp* inferno_create();
void inferno_cleanup(InfernoApp* app);
void inferno_preset_gui(InfernoApp* app);
void inferno_move_input(InfernoApp* app, std::chrono::duration<double> delta);
void inferno_stop_move_input(InfernoApp* app);

bool inferno_pre(InfernoApp* app);
void inferno_end(InfernoApp* app);
int inferno_run(InfernoApp* app);

} // namespace inferno
