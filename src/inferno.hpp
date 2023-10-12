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
    glm::vec2 MouseDelta = {0.0f, 0.0f};
    uint8_t MovementDelta = 0;
} InfernoInput;

typedef struct InfernoApp {
    InfernoInput* Input;
    scene::Scene* Scene;
    graphics::PreviewRenderer* PreviewRenderer;
    // graphics::RayRenderer* RayRenderer;
} InfernoApp;

InfernoApp* inferno_create();
void inferno_cleanup(InfernoApp* app);
void inferno_preset_gui(InfernoApp* app);
void inferno_move_input(InfernoApp* app);
void inferno_stop_move_input(InfernoApp* app);
int inferno_run(InfernoApp* app);

} // namespace inferno
