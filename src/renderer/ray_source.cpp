#include "ray_source.hpp"

#include <iostream>

#include <maths.hpp>

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
    // const float aspect =  mReferenceCamera->getRayViewport().x / mReferenceCamera->getRayViewport().y;
    // float scale = tan(mReferenceCamera->FOV / 2.0f * helpers::PI / 180.0f);
}

ReferencedRayField RaySource::getInitialRays(bool MSAA)
{
    if (mReferenceCamera->didUpdate())
    {
        this->generate();
    }

    RayField field;
    field.reserve(mReferenceCamera->getRayViewport().x * mReferenceCamera->getRayViewport().y);

    const float aspect =  mReferenceCamera->getRayViewport().x / mReferenceCamera->getRayViewport().y;
    float scale = tan(mReferenceCamera->FOV / 2.0f * helpers::PI / 180.0f);
    
    glm::mat4 cameraToWorld = mReferenceCamera->getCameraLook();
    glm::vec3 origin = mReferenceCamera->Position;

    std::unordered_map<uint32_t, glm::ivec2> reference;

    uint32_t i = 0;
    for (int x = 0; x < mReferenceCamera->getRayViewport().x; x++)
    for (int y = 0; y < mReferenceCamera->getRayViewport().y; y++)
    {
        float Px = (2.0f * ((x + 0.5f) /  mReferenceCamera->getRayViewport().x) - 1.0f) * scale * aspect;
        float Py = (1.0f - 2.0f * ((y + 0.5f) /  mReferenceCamera->getRayViewport().y) * scale); 

        Ray* ray = new Ray{};
        ray->Origin = origin;
        ray->Direction = glm::normalize((glm::vec4(Px, Py, -1.0f, 1.0f) * cameraToWorld));
        ray->Reference = i;
        reference[i] = {x, y};

        field.push_back(ray);
        i++;
    }

    return { field, reference };
}
