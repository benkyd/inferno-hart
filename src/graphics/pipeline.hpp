#pragma once

#include "graphics.hpp"

namespace inferno::graphics {

struct GraphicsDevice;
struct SwapChain;

typedef struct Pipeline {
    GraphicsDevice* Device;
    SwapChain* Swap;
} Pipeline;

Pipeline* pipeline_create(GraphicsDevice* device);
void pipeline_cleanup(Pipeline* pipeline);

}
