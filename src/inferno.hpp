#pragma once

#include "graphics.hpp"

#include <memory>

namespace inferno {

typedef struct InfernoInput {
    glm::vec2 MouseDelta;
    uint8_t MovementDelta;
} InfernoInput;

typedef struct InfernoApp {
    std::unique_ptr<InfernoInput> Input;
} InfernoApp;

std::unique_ptr<InfernoApp> inferno_create();
void inferno_cleanup(std::unique_ptr<InfernoApp>& app);
void inferno_preset_gui(std::unique_ptr<InfernoApp>& app);
void inferno_move_input(std::unique_ptr<InfernoApp>& app);
void inferno_stop_move_input(std::unique_ptr<InfernoApp>& app);
int inferno_run(std::unique_ptr<InfernoApp>& app);

}
