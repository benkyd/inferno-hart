#pragma once

#include "graphics.hpp"

namespace inferno::graphics {

struct GraphicsDevice;

typedef struct Pipeline {
    VkPipelineLayout Layout;
    VkPipeline Pipeline;

    GraphicsDevice* Device;
} Pipeline;

}
