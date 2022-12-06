#pragma once

#include <vector>

#include <graphics.hpp>

#include <tracing/ray.hpp>

namespace inferno {

class Camera;

class RaySource
{
public:
    RaySource(Camera* camera);
    ~RaySource();

    void generate();
    RayField getInitialRays(bool MSAA);

private:
    Camera* mReferenceCamera;
};

}
