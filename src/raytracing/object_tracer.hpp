#pragma once

#include <scene/object.hpp>
#include <scene/mesh.hpp>

#include <raytracing/hit.hpp>
#include <raytracing/ray.hpp>

namespace inferno::graphics::rays {

HitInfo*  object_ray_collide(scene::SceneObject* object, Ray* ray);

}

