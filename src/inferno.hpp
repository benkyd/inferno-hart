#pragma once

#include "graphics.hpp"

#include <singleton.hpp>

namespace inferno::core {

class Window;
class HHM;

typedef struct InfernoInput {
    glm::vec2 MouseDelta;
    uint8_t MovementDelta;
} InfernoInput;

typedef struct InfernoApp {
    Window* Win;
    InfernoInput* Input;
} InfernoApp;

InfernoApp* create_inferno();
void cleanup_inferno(InfernoApp* app);
void preset_gui(InfernoApp* app);
void move_input(InfernoApp* app);
void stop_move_input(InfernoApp* app);
int run(InfernoApp* app);

}

