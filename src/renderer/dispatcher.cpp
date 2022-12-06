#include "dispatcher.hpp"

#include "hart_module.hpp"
#include "renderer.hpp"


using namespace inferno;

RenderDispatcher::RenderDispatcher()
{
    mHHM = new HHM();
    mRenderer = new RayRenderer(mHHM);
}

RenderDispatcher::~RenderDispatcher()
{

}

RayRenderer* RenderDispatcher::getRenderer()
{
    return mRenderer;
}

HHM* RenderDispatcher::getTopModule()
{
    return mHHM;
}

void RenderDispatcher::startProgression()
{

}

void RenderDispatcher::stopProgression()
{

}

GLuint RenderDispatcher::getLatestTexture()
{

}
