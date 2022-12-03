#include "renderer.hpp"

#include <iostream>

#include <scene/camera.hpp>
#include <scene/scene.hpp>

#include "hart_module.hpp"
#include "ray_source.hpp"

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
    mIface->newScene(scene);
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
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, mRenderTargetTexture);
    return mRenderTargetTexture;
}

void RayRenderer::prepare()
{
    assert(mCurrentScene != NULL);
    mIface->newScene(mCurrentScene);
}

void RayRenderer::draw()
{
    RayField startRays = mRaySource->getInitialRays(true);
    mIface->startTrace(&startRays);

    for (int x = 0; x < mRenderTargetSize.x; x++)
    for (int y = 0; y < mRenderTargetSize.y; y++)
    {
        mTarget[y * mRenderTargetSize.x + x] = { startRays[y * mRenderTargetSize.x + x]->Direction, 1.0f };
    }

    glBindTexture(GL_TEXTURE_2D, mRenderTargetTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mRenderTargetSize.x, mRenderTargetSize.y, 0, GL_RGBA, GL_FLOAT, mTarget);
    glBindTexture(GL_TEXTURE_2D, 0);

    for (auto* ray : startRays)
    {
        delete ray;
    }
}
