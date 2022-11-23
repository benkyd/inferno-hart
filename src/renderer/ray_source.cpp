#include "ray_source.hpp"

#include <scene/camera.hpp>

#include <tracing/ray.hpp>

using namespace inferno;

RaySource::RaySource(Camera* camera)
    : mReferenceCamera(camera)
{
    this->generate();
}

RaySource::~RaySource()
{

}

void RaySource::generate()
{

}

RayField RaySource::getInitialRays(bool MSAA)
{
    if (mReferenceCamera->didUpdate())
    {
        this->generate();
    }

    for (int x = 0; x < mReferenceCamera->getRayViewport().x; x++)
    for (int y = 0; y < mReferenceCamera->getRayViewport().y; y++)

    return RayField{};
}
