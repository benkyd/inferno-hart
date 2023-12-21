#include "renderer.hpp"

#include <graphics.hpp>
#include <graphics/rendertarget.hpp>
#include <graphics/vkrenderer.hpp>

#include <raytracing/hit.hpp>
#include <raytracing/object_tracer.hpp>
#include <raytracing/ray.hpp>

#include <scene/camera.hpp>
#include <scene/mesh.hpp>
#include <scene/scene.hpp>

// #include "preview/debug.hpp"
#include "ray_source.hpp"

#include <yolo/yolo.hpp>

#include <scene/camera.hpp>

#include <cstring>
#include <iostream>

namespace inferno::graphics {

RayRenderer* rayr_create(VulkanRenderer* renderer, scene::Scene* scene)
{
    RayRenderer* ray = new RayRenderer;

    ray->Scene = scene;
    ray->Renderer = renderer;

    auto camera = scene::scene_get_camera(scene);
    glm::ivec2 viewport = camera_ray_get_viewport(camera);
    ray->Viewport = { .extent
        = { static_cast<uint32_t>(viewport.x), static_cast<uint32_t>(viewport.y) } };

    ray->RenderData
        = new glm::fvec4[ray->Viewport.extent.width * ray->Viewport.extent.height];
    memset(ray->RenderData, 0,
        ray->Viewport.extent.width * ray->Viewport.extent.height * sizeof(glm::fvec4));

    // TODO: We need to upload RenderData to the GPU / maybe do a quad pass ? I think
    // ImGui handels that

    // ray->RayRenderTarget = graphics::rendertarget_create()

    return ray;
}

void rayr_cleanup(RayRenderer* renderer) { delete[] renderer->RenderData; }

void rayr_draw_ui(RayRenderer* renderer) { }

void rayr_set_viewport(RayRenderer* renderer, Camera* camera)
{
    auto viewport = camera_ray_get_viewport(camera);
    renderer->Viewport.offset.x = 0;
    renderer->Viewport.offset.y = 0;
    renderer->Viewport.extent.width = viewport.x;
    renderer->Viewport.extent.height = viewport.y;
    renderer->HasViewportChanged = true;
}

GLuint rayr_get_rendered_texture(RayRenderer* renderer)
{
    // return renderer->RenderTargetTexture;
}

glm::fvec4* rayr_get_render_data(RayRenderer* renderer) { return renderer->RenderData; }

void rayr_prepare(RayRenderer* renderer)
{
    // TODO: This is TEMP
    memset(renderer->RenderData, 0,
        renderer->Viewport.extent.width * renderer->Viewport.extent.height
            * sizeof(glm::fvec4));

    assert(renderer->Scene != nullptr);
    if (scene::scene_did_update(renderer->Scene)) {
        yolo::debug("Scene updated, rebuilding acceleration structure");
        // renderer->AccelerationInterface->newScene(renderer->CurrentScene);
    }
}

void rayr_draw(RayRenderer* renderer)
{
    rayr_prepare(renderer);

    scene::scene_frame_tick(renderer->Scene);
    // TODO: Rays should definately be bump allocated if possible, this is KBs of
    // ray data and nothing else being reallocated every frame for no reason
    rays::ReferencedRayField startRays
        = rays::generate_initial_rays(scene::scene_get_camera(renderer->Scene), true);

    // debug_draw_line({ 0, 0, 0 }, { 0, 0, 1 }, { 1, 0, 0 });

#pragma omp parallel for
    for (int x = 0; x < renderer->Viewport.extent.width; x++) {
        for (int y = 0; y < renderer->Viewport.extent.height; y++) {
            rays::Ray* ray = startRays.Field[x * renderer->Viewport.extent.height + y];
            rays::HitInfo* closest_hit = nullptr;

            for (auto& obj : scene::scene_get_renderables(renderer->Scene)) {
                rays::HitInfo* hit = rays::object_ray_collide(obj, ray);
                if (hit->Did) {
                    if (closest_hit == nullptr) {
                        closest_hit = hit;
                    } else {
                        bool is_closer = hit->Distance < closest_hit->Distance;
                        if (is_closer) {
                            delete closest_hit;
                            closest_hit = hit;
                        } else {
                            delete hit;
                        }
                    }
                }
                if (hit->Did) {
                    glm::vec3 hit_distance = glm::vec3 { hit->Distance };
                    hit_distance /= 10;
                    renderer->RenderData[y * renderer->Viewport.extent.width + x]
                        = { hit_distance, 1.0 };
                }
                delete hit;
            }
        }
    }
}
//
// void RayRenderer::computeHit(HitInfo* info)
// {
//     static float mind = 100000.0f;
//     static float maxd = 0.0f;
//     // TODO: Make sure signal is started
//     if (!(*mCurrentRefTable).count(info->Caller->Reference)) {
//         yolo::warn("Why is the ray not in the map?!");
//         return;
//     }
//     glm::ivec2 pos = (*mCurrentRefTable)[info->Caller->Reference];
//     float d = info->Distance;
//     if (d < mind)
//         mind = d;
//     if (d > maxd)
//         maxd = d;
//     float n = (d - mind) / (maxd - mind);
//     mTarget[pos.y * mRenderTargetSize.x + pos.x] = { n, n, n, 1.0f };
// }

}
