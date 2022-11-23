#pragma once

#include <vector>

#include <graphics.hpp>

#include <tracing/ray.hpp>

namespace inferno {

typedef std::vector<Ray*> RayField;

class Camera;

class RaySource
{
public:
    RaySource();
    ~RaySource();

    void cameraUpdate(Camera* camera);
    void generate();

    RayField getInitialRays(bool MSAA);

private:
    Camera* mReferenceCamera;
};

}
