#include "renderer.hpp"

#include <scene/camera.hpp>
#include <scene/scene.hpp>

using namespace inferno;

RasterizeRenderer::RasterizeRenderer()
{

}

RasterizeRenderer::~RasterizeRenderer()
{

}

void RasterizeRenderer::setScene(Scene* scene)
{
    mCurrentScene = scene;
}

void RasterizeRenderer::setTarget(GLuint renderTarget)
{

}

void RasterizeRenderer::prepare()
{

}

void RasterizeRenderer::draw()
{

}
