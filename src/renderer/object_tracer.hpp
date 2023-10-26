#pragma once 

#include <scene/object.hpp>
#include <scene/mesh.hpp>
#include <renderer/hit.hpp>
#include <renderer/ray.hpp>

namespace inferno::graphics::rays {

HitInfo*  object_ray_collide(scene::SceneObject* object, Ray* ray);

}

