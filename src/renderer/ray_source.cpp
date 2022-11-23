#include "ray_source.hpp"

using namespace inferno;

RaySource::RaySource()
{

}

RaySource::~RaySource()
{

}

void RaySource::cameraUpdate(Camera* camera)
{
    mReferenceCamera = camera;
    
}

void RaySource::generate()
{

}

RayField RaySource::getInitialRays(bool MSAA)
{
    return RayField{};
}
