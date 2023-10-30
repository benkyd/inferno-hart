#include "renderer.hpp"

#include <graphics.hpp>

#include <renderer/hit.hpp>
#include <renderer/object_tracer.hpp>
#include <renderer/ray.hpp>
#include <scene/camera.hpp>
#include <scene/mesh.hpp>
#include <scene/scene.hpp>

#include "ray_source.hpp"

#include <yolo/yolo.hpp>

#include <scene/camera.hpp>

#include <cstring>
#include <iostream>

namespace inferno::graphics {

RayRenderer* rayr_create(scene::Scene* scene)
{
    RayRenderer* renderer = new RayRenderer;

    renderer->Scene = scene;

    auto camera = scene::scene_get_camera(scene);
    auto viewport = camera_ray_get_viewport(camera);
    renderer->Viewport = viewport;

    yolo::debug("Raytracing Rendering {}x{} viewport", renderer->Viewport.x, renderer->Viewport.y);
    renderer->RenderData = new glm::fvec4[renderer->Viewport.x * renderer->Viewport.y];
    memset(renderer->RenderData, 0, renderer->Viewport.x * renderer->Viewport.y * sizeof(glm::fvec4));

    glGenTextures(1, &renderer->RenderTargetTexture);
    glBindTexture(GL_TEXTURE_2D, renderer->RenderTargetTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderer->Viewport.x, renderer->Viewport.y, 0, GL_RGBA, GL_FLOAT, renderer->RenderData);

    glBindTexture(GL_TEXTURE_2D, 0);

    return renderer;
}

void rayr_cleanup(RayRenderer* renderer)
{
    delete[] renderer->RenderData;
}

void rayr_draw_ui(RayRenderer* renderer)
{
}

void rayr_set_viewport(RayRenderer* renderer, Camera* camera)
{
    auto viewport = camera_ray_get_viewport(camera);
    renderer->Viewport = viewport;

    delete renderer->RenderData;
    renderer->RenderData = new glm::fvec4[renderer->Viewport.x * renderer->Viewport.y];

    glGenTextures(1, &renderer->RenderTargetTexture);
    glBindTexture(GL_TEXTURE_2D, renderer->RenderTargetTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderer->Viewport.x, renderer->Viewport.y, 0, GL_RGBA, GL_FLOAT, renderer->RenderData);

    glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint rayr_get_rendered_texture(RayRenderer* renderer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, renderer->RenderTargetTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderer->Viewport.x, renderer->Viewport.y, 0, GL_RGBA, GL_FLOAT, renderer->RenderData);
    return renderer->RenderTargetTexture;
}

glm::fvec4* rayr_get_render_data(RayRenderer* renderer)
{
    return renderer->RenderData;
}

void rayr_prepare(RayRenderer* renderer)
{
    // TODO: This is TEMP
    memset(renderer->RenderData, 0, renderer->Viewport.x * renderer->Viewport.y * sizeof(glm::fvec4));

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
    rays::ReferencedRayField startRays = rays::generate_initial_rays(scene::scene_get_camera(renderer->Scene), true);

#pragma omp parallel for
    for (int x = 0; x < renderer->Viewport.x; x++) {
        for (int y = 0; y < renderer->Viewport.y; y++) {
            rays::Ray* ray = startRays.Field[x * renderer->Viewport.y + y];
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
                    renderer->RenderData[y * renderer->Viewport.x + x] = { hit_distance, 1.0 };
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
