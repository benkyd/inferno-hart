#pragma once

#include <hart_graphics.hpp>

namespace inferno {

struct Ray
{
    glm::vec3 Origin;
    glm::vec3 Direction;
    uint32_t Reference;
};

}
