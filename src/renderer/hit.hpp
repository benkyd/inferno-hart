#pragma once

#include <hart_graphics.hpp>

namespace inferno::graphics::rays {

class Ray;

struct HitInfo
{
    Ray* Caller;
    glm::vec2 UV;
    glm::vec3 SurfaceNormal;
    float Distance;
};

}
