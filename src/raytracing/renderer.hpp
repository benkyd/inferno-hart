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

class VulkanRenderer;
class DynamicCPUTarget;

class Camera;
class HitInfo;
class RaySource;
class RenderDispatcher;

typedef struct RayRenderer {
    VulkanRenderer* Renderer;

    // TODO: Can this be direct to GPU?
    // NOTE: Probably not
    glm::fvec4* RenderData = nullptr;
    DynamicCPUTarget* RayRenderTarget;

    scene::Scene* Scene = nullptr;

    VkRect2D Viewport;
    bool HasViewportChanged;
} RayRenderer;

// Expects complete scene as it will need to bake a lot of features
RayRenderer* rayr_create(VulkanRenderer* renderer, scene::Scene* scene);
void rayr_cleanup(RayRenderer* renderer);

void rayr_draw_ui(RayRenderer* renderer);
void rayr_set_viewport(RayRenderer* renderer, Camera* camera);

DynamicCPUTarget* rayr_get_target(RayRenderer* renderer);
glm::fvec4* rayr_get_render_data(RayRenderer* renderer);

void rayr_prepare(RayRenderer* renderer);
void rayr_draw(RayRenderer* renderer);

void raryr_compute_hit(HitInfo* info);

} // namespace inferno::graphics
