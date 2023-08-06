#include "renderer.hpp"

#include <graphics.hpp>

#include <scene/camera.hpp>
#include <scene/scene.hpp>
#include <scene/mesh.hpp>
#include <tracing/ray.hpp>
#include <tracing/hit.hpp>

#include "hart_module.hpp"
#include "ray_source.hpp"

#include <yolo/yolo.hpp>

#include <iostream>

namespace inferno::graphics {

std::unique_ptr<RayRenderer> rayr_create(glm::ivec2 viewport, HHM* accelIface)
{
    std::unique_ptr<RayRenderer> renderer = std::make_unique<RayRenderer>();
    renderer->RenderTargetSize = viewport;
    renderer->RenderData = new glm::fvec4[renderer->RenderTargetSize.x * renderer->RenderTargetSize.y];

    glGenTextures(1, &renderer->RenderTargetTexture);
    glBindTexture(GL_TEXTURE_2D, renderer->RenderTargetTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderer->RenderTargetSize.x, renderer->RenderTargetSize.y, 0, GL_RGBA, GL_FLOAT, renderer->RenderData);

    glBindTexture(GL_TEXTURE_2D, 0);

    return renderer;
}

void rayr_cleanup(std::unique_ptr<RayRenderer>& renderer)
{
    delete[] renderer->RenderData;
}

void rayr_set_scene(std::unique_ptr<RayRenderer>& renderer, std::shared_ptr<scene::Scene> scene)
{
    renderer->CurrentScene = scene;
    if (renderer->RaySource != nullptr)
    {
        delete renderer->RaySource;
    }
    // renderer->RaySource = new RaySource(scene->getCamera());
    // the scene will be sent to the module on prepare
    // as it did update during initialisation

    // mIface->newScene(scene);
}

void rayr_set_viewport(std::unique_ptr<RayRenderer> &renderer, glm::ivec2 size)
{
    renderer->RenderTargetSize = size;
}

glm::ivec2 rayr_get_viewport(std::unique_ptr<RayRenderer> &renderer)
{
    return renderer->RenderTargetSize;
}

GLuint rayr_get_rendered_texture(std::unique_ptr<RayRenderer> &renderer)
{
    std::lock_guard<std::mutex> lock(renderer->RenderDataMutex);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, renderer->RenderTargetTexture);
    return renderer->RenderTargetTexture;
}

glm::fvec4* rayr_get_render_data(std::unique_ptr<RayRenderer> &renderer)
{
    std::lock_guard<std::mutex> lock(renderer->RenderDataMutex);
    return renderer->RenderData;
}

void rayr_prepare(std::unique_ptr<RayRenderer> &renderer)
{
    assert(renderer->CurrentScene != nullptr);
    // here, scene_did_update takes a unique_ptr, but we have a shared_ptr
    // so we need to call unique() to get the unique_ptr but that will error
    // with non-const ltype so we need to const_cast it
    if (scene::scene_did_update(renderer->CurrentScene.unique()))
    {
        yolo::debug("New Scene!");
        // renderer->AccelerationInterface->newScene(renderer->CurrentScene);
    }
}

void rayr_draw(std::unique_ptr<RayRenderer> &renderer)
{
    scene::scene_frame_tick(renderer->CurrentScene.unique());
    // TODO: Rays should definately be bump allocated if possible, this is KBs of
    // ray data and nothing else being reallocated every frame for no reason
    ReferencedRayField startRays = mRaySource->getInitialRays(true);

    for (int x = 0; x < mRenderTargetSize.x; x++)
    for (int y = 0; y < mRenderTargetSize.y; y++)
    {
        mTarget[y * mRenderTargetSize.x + x] = { 0.1f, 0.1f, 0.1f, 1.0f };
    }
    mCurrentRefTable = &startRays.Reference;

    // before we start we now want to check that it hasn't been force-stopped
    mIface->startTrace(startRays.Field);

    yolo::info("Sample complete");

    for (auto* ray : startRays.Field)
    {
        delete ray;
    }
}

void RayRenderer::computeHit(HitInfo* info)
{
    static float mind = 100000.0f;
    static float maxd = 0.0f; 
    // TODO: Make sure signal is started
    if (!(*mCurrentRefTable).count(info->Caller->Reference))
    {
        yolo::warn("Why is the ray not in the map?!");
        return;
    }
    glm::ivec2 pos = (*mCurrentRefTable)[info->Caller->Reference];
    float d = info->Distance; 
    if (d < mind) mind = d;
    if (d > maxd) maxd = d;
    float n = (d - mind) / (maxd - mind);
    mTarget[pos.y * mRenderTargetSize.x + pos.x] = { n, n, n, 1.0f};
}
    void mHaultWait();
    std::unordered_map<uint32_t, glm::ivec2>* mCurrentRefTable;


}
