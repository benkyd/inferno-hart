#pragma once

#include "graphics.hpp"
#include "scene/scene.hpp"
#include "scene/camera.hpp"
// #include "renderer/renderer.hpp"
#include "preview_renderer/renderer.hpp"

#include <memory>

namespace inferno {

namespace graphics {
    struct Camera;
}

namespace scene {
    struct Scene;
}

typedef struct InfernoInput {
    glm::vec2 MouseDelta;
    uint8_t MovementDelta;
} InfernoInput;

typedef struct InfernoApp {
    std::unique_ptr<InfernoInput> Input;
    std::unique_ptr<scene::Scene> Scene;
    std::unique_ptr<graphics::PreviewRenderer> PreviewRenderer;
    // std::unique_ptr<graphics::RayRenderer> RayRenderer;
} InfernoApp;

std::unique_ptr<InfernoApp> inferno_create();
void inferno_cleanup(std::unique_ptr<InfernoApp>& app);
void inferno_preset_gui(std::unique_ptr<InfernoApp>& app);
void inferno_move_input(std::unique_ptr<InfernoApp>& app);
void inferno_stop_move_input(std::unique_ptr<InfernoApp>& app);
int inferno_run(std::unique_ptr<InfernoApp>& app);

} // namespace inferno
