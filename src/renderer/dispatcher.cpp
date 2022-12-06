#include "dispatcher.hpp"

#include "hart_module.hpp"
#include "renderer.hpp"

#include <spdlog/spdlog.h>

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
        spdlog::debug("Sample complete");
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
    return mRenderer->getRenderedTexture();
}
