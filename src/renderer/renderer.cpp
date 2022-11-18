#include "renderer.hpp"

#include "colour.hpp"

#include <scene/camera.hpp>
#include <scene/scene.hpp>

using namespace inferno;

RayRenderer::RayRenderer()
{
    mTarget = new Colour[mRenderTargetSize.x * mRenderTargetSize.y];

    glGenTextures(1, &mRenderTargetTexture);
    glBindTexture(GL_TEXTURE_2D, mRenderTargetTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mRenderTargetSize.x, mRenderTargetSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, mTarget);

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

}

void RayRenderer::draw()
{
    for (int x = 0; x < mRenderTargetSize.x; x++)
    for (int y = 0; y < mRenderTargetSize.y; y++)
    {
        mTarget[y * mRenderTargetSize.x + x] = { 0xFF * (uint8_t)x, 0x00, 0xFF * (uint8_t)y, 0xFF };
    }

    glBindTexture(GL_TEXTURE_2D, mRenderTargetTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mRenderTargetSize.x, mRenderTargetSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, mTarget);
    glBindTexture(GL_TEXTURE_2D, 0);
}
