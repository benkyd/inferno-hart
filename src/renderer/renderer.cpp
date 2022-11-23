#include "renderer.hpp"

#include <scene/camera.hpp>
#include <scene/scene.hpp>

#include "hart_module.hpp"
#include "ray_source.hpp"

using namespace inferno;

RayRenderer::RayRenderer(HHM* accelIface)
    : mIface(accelIface)
    , mRaySource(new RaySource)
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
    
    mRaySource->cameraUpdate(mCurrentScene->getCamera());
}

void RayRenderer::draw()
{
    RayField startRays = mRaySource->getInitialRays(true);

    for (int x = 0; x < mRenderTargetSize.x; x++)
    for (int y = 0; y < mRenderTargetSize.y; y++)
    {
        mTarget[y * mRenderTargetSize.x + x] = { 0.0f + ((float)x / (float)mRenderTargetSize.x),  0.0f + ((float)y / (float)mRenderTargetSize.y), 1.0f, 1.0f };
    }

    glBindTexture(GL_TEXTURE_2D, mRenderTargetTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mRenderTargetSize.x, mRenderTargetSize.y, 0, GL_RGBA, GL_FLOAT, mTarget);
    glBindTexture(GL_TEXTURE_2D, 0);
}
