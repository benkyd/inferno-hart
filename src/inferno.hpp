#pragma once

#include "graphics.hpp"

#include <memory>

namespace inferno {

class Window;
class HHM;

typedef struct InfernoInput {
    glm::vec2 MouseDelta;
    uint8_t MovementDelta;
} InfernoInput;

typedef struct InfernoApp {
    InfernoInput* Input;
} InfernoApp;

InfernoApp* inferno_create();
void inferno_cleanup(InfernoApp* app);
void inferno_preset_gui(InfernoApp* app);
void inferno_move_input(InfernoApp* app);
void inferno_stop_move_input(InfernoApp* app);
int inferno_run(InfernoApp* app);

}
