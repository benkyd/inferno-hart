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

class Camera;
class HitInfo;
class RaySource;
class RenderDispatcher;

typedef struct RayRenderer {
    glm::ivec2 Viewport;

    // TODO: Can this be direct to GPU?
    // NOTE: Probably not
    glm::fvec4* RenderData = nullptr;
    GLuint RenderTargetTexture = 0;

    scene::Scene* Scene = nullptr;
} RayRenderer;

// Expects complete scene
RayRenderer* rayr_create(scene::Scene* scene);
void rayr_cleanup(RayRenderer* renderer);

void rayr_draw_ui(RayRenderer* renderer);
void rayr_set_viewport(RayRenderer* renderer, Camera* camera);

GLuint rayr_get_rendered_texture(RayRenderer* renderer);
glm::fvec4* rayr_get_render_data(RayRenderer* renderer);

void rayr_draw(RayRenderer* renderer);

void raryr_compute_hit(HitInfo* info);

} // namespace inferno::graphics
