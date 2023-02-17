#include "renderer.hpp"

#include <graphics.hpp>

#include <scene/camera.hpp>
#include <scene/scene.hpp>
#include <scene/mesh.hpp>
#include <tracing/ray.hpp>
#include <tracing/hit.hpp>

#include "hart_module.hpp"
#include "ray_source.hpp"

#include <spdlog/spdlog.h>

#include <iostream>

using namespace inferno;

RayRenderer::RayRenderer(HHM* accelIface)
    : mIface(accelIface)
{
    mTarget = new glm::fvec4[mRenderTargetSize.x * mRenderTargetSize.y];

    glGenTextures(1, &mRenderTargetTexture);
    glBindTexture(GL_TEXTURE_2D, mRenderTargetTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mRenderTargetSize.x, mRenderTargetSize.y, 0, GL_RGBA, GL_FLOAT, mTarget);

    glBindTexture(GL_TEXTURE_2D, 0);
}

RayRenderer::~RayRenderer()
{
    delete[] mTarget;
}

void RayRenderer::setScene(Scene* scene)
{
    mCurrentScene = scene;
    if (mRaySource != nullptr)
    {
        delete mRaySource;
    }
    mRaySource = new RaySource(scene->getCamera());
    // the scene will be sent to the module on prepare
    // as it did update during initialisation

    // mIface->newScene(scene);
}

void RayRenderer::setTargetSize(glm::ivec2 size)
{
    mRenderTargetSize = size;
}

glm::ivec2 RayRenderer::getTargetSize()
{
    return mRenderTargetSize;
}

GLuint RayRenderer::getRenderedTexture()
{
    std::lock_guard<std::mutex> lock(this->_RenderData);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, mRenderTargetTexture);
    return mRenderTargetTexture;
}

glm::fvec4* RayRenderer::getRenderData()
{
    return mTarget;
}

void RayRenderer::prepare()
{
    assert(mCurrentScene != nullptr);
    if (mCurrentScene->didUpdate())
    {
        spdlog::debug("New Scene!");
        mIface->newScene(mCurrentScene);
    }
}

void RayRenderer::draw()
{
    mCurrentScene->newFrame();
    // TODO: Rays should definately be bump allocated if possible, this is KBs of
    // ray data and nothing else being reallocated every frame for no reason
    ReferencedRayField startRays = mRaySource->getInitialRays(true);

    for (int x = 0; x < mRenderTargetSize.x; x++)
    for (int y = 0; y < mRenderTargetSize.y; y++)
    {
        mTarget[y * mRenderTargetSize.x + x] = { 1.0f, 0.0f, 0.0f, 1.0f };
    }
    mCurrentRefTable = &startRays.Reference;

    // before we start we now want to check that it hasn't been force-stopped
    mIface->startTrace(startRays.Field);

    spdlog::info("Sample complete");

    for (auto* ray : startRays.Field)
    {
        delete ray;
    }
}

void RayRenderer::computeHit(HitInfo* info)
{
    // TODO: Make sure signal is started
    if (!(*mCurrentRefTable).count(info->Caller->Reference))
    {
        spdlog::warn("Why is the ray not in the map?!");
        return;
    }
    glm::ivec2 pos = (*mCurrentRefTable)[info->Caller->Reference];
    float d = info->Distance; 
    mTarget[pos.y * mRenderTargetSize.x + pos.x] = { d, d, d, 1.0f };
}
