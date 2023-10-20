#pragma once

#include <vector>
#include <unordered_map>

#include <graphics.hpp>

#include <renderer/ray.hpp>

namespace inferno::graphics {
    class Camera;
}

namespace inferno::graphics::rays {

struct ReferencedRayField {
    RayField Field;
    std::unordered_map<uint32_t, glm::ivec2> Reference;
};

ReferencedRayField generate_initial_rays(Camera* camera, bool MSAA);

}
