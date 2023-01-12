#pragma once

#include <hart_graphics.hpp>

namespace inferno {

class Ray;

struct HitInfo
{
    Ray* Caller;
    // indicie of hit ^ mesh idx of hit
    int Identifier;
    glm::vec2 UV;
    glm::vec3 SurfaceNormal;
    float Distance;
};

}
