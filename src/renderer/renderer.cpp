#include "renderer.hpp"

#include <scene/camera.hpp>
#include <scene/scene.hpp>

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
    std::lock_guard<std::mutex> lock(this->_mTarget);
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
    assert(mCurrentScene != NULL);
    if (mCurrentScene->didUpdate())
    {
        mIface->newScene(mCurrentScene);
    }
}

void RayRenderer::draw()
{
    mCurrentScene->newFrame();

    RayField startRays = mRaySource->getInitialRays(true);
    mIface->startTrace(startRays);

    // hault wait for the module to finish
    bool frameStatus = false;
    while (!frameStatus)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        switch(mIface->getModuleState())
        {
            case EModuleState::Bad:
                spdlog::error("MODULE STATE BAD");
            case EModuleState::Build:
            case EModuleState::Trace:
                spdlog::debug("MODULE BUSY..");
                break;
            case EModuleState::Ready:
                frameStatus = true;
                break;
        }
    }

    spdlog::info("Sample complete");

    for (auto* ray : startRays)
    {
        delete ray;
    }
}
