#include "dispatcher.hpp"

#include "hart_module.hpp"
#include "renderer.hpp"

#include <spdlog/spdlog.h>

#include <mutex>
#include <chrono>

using namespace inferno;

RenderDispatcher::RenderDispatcher()
    : mRenderWorker{}
{
    mHHM = new HHM();
    mRenderer = new RayRenderer(mHHM);
}

RenderDispatcher::~RenderDispatcher()
{
    mDoWork = false;
    mJoin = true;
    mRenderWorker.join();
}

RayRenderer* RenderDispatcher::getRenderer()
{
    return mRenderer;
}

HHM* RenderDispatcher::getTopModule()
{
    return mHHM;
}

void renderWorker(RayRenderer* renderer, std::atomic<bool>* doWork, std::atomic<bool>* join)
{
    while (!*join)
    {
        if (!*doWork) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue; // stall
        }
        renderer->prepare();
        renderer->draw();
    }
}

void RenderDispatcher::startProgression()
{
    if (!mRenderWorker.joinable())
    {
        mRenderWorker = std::thread{ &renderWorker, this->mRenderer, &mDoWork, &mJoin };
    }
    mDoWork = true;
}

void RenderDispatcher::stopProgression()
{
    mDoWork = false;
}

bool RenderDispatcher::progressionStatus()
{
    return mDoWork;
}

GLuint RenderDispatcher::getLatestTexture()
{
    std::lock_guard<std::mutex> lock(mRenderer->_mTarget);
    glBindTexture(GL_TEXTURE_2D, mRenderer->mRenderTargetTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mRenderer->mRenderTargetSize.x, mRenderer->mRenderTargetSize.y, 0, GL_RGBA, GL_FLOAT, mRenderer->mTarget);
    glBindTexture(GL_TEXTURE_2D, 0);
    return mRenderer->mRenderTargetTexture;
}
