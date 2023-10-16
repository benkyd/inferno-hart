#pragma once

#include <graphics.hpp>

#include <condition_variable>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace inferno::scene {
struct Scene;
}

namespace inferno::graphics {

class HHM;

class HitInfo;
class RaySource;
class RenderDispatcher;

typedef struct RayRenderer {
    glm::ivec2* Viewport;

    // TODO: Can this be direct 2 GPU?
    glm::fvec4* RenderData = nullptr;
    GLuint RenderTargetTexture = 0;

    // Internal stuffs
    RaySource* RaySource = nullptr;
} RayRenderer;

RayRenderer* rayr_create(glm::ivec2 viewport, HHM* accelIface);
void rayr_cleanup(RayRenderer* renderer);

void rayr_set_viewport(RayRenderer* renderer, glm::ivec2 size);

GLuint rayr_get_rendered_texture(RayRenderer* renderer);
glm::fvec4* rayr_get_render_data(RayRenderer* renderer);

void rayr_draw(RayRenderer* renderer, scene::Scene* scene);

void raryr_compute_hit(HitInfo* info);

} // namespace inferno::graphics
