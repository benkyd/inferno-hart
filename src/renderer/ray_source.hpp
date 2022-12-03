#pragma once

#include <vector>

#include <graphics.hpp>

#include <tracing/ray.hpp>

namespace inferno {

using RayField = std::vector<Ray*>;

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
