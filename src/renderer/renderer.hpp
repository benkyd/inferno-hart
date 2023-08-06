#pragma once

#include <graphics.hpp>

#include <condition_variable>
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
    glm::fvec4* RenderData = nullptr;
    GLuint RenderTargetTexture = 0;

    // TODO: Put this inside an internal struct
    std::mutex RenderDataMutex;
    std::condition_variable RenderPause;
    // TODO: End

    std::shared_ptr<scene::Scene> CurrentScene;
    // std::shared_ptr<const Viewport> RenderTargetSize;
    glm::ivec2 RenderTargetSize;

    HHM* AccelerationInterface = nullptr;
    RenderDispatcher* Dispatcher = nullptr;
    RaySource* RaySource = nullptr;
} RayRenderer;

std::unique_ptr<RayRenderer> rayr_create(glm::ivec2 viewport, HHM* accelIface);
void rayr_cleanup(std::unique_ptr<RayRenderer>& renderer);
void rayr_set_scene(std::unique_ptr<RayRenderer>& renderer, std::shared_ptr<scene::Scene> scene);
void rayr_set_viewport(std::unique_ptr<RayRenderer>& renderer, glm::ivec2 size);
glm::ivec2 rayr_get_viewport(std::unique_ptr<RayRenderer>& renderer);
GLuint rayr_get_rendered_texture(std::unique_ptr<RayRenderer>& renderer);
glm::fvec4* rayr_get_render_data(std::unique_ptr<RayRenderer>& renderer);
void rayr_prepare(std::unique_ptr<RayRenderer>& renderer);
void rayr_draw(std::unique_ptr<RayRenderer>& renderer);

void raryr_compute_hit(HitInfo* info);

}
