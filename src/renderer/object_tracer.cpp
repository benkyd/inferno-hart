#include "object_tracer.hpp"
#include "scene/object.hpp"

#include <yolo/yolo.hpp>

namespace inferno::graphics::rays {

HitInfo* object_ray_collide(scene::SceneObject* object, Ray* ray)
{
    HitInfo* info = nullptr;
    for (auto* mesh : scene::scene_object_get_meshs(object)) {
        // extract triangles and loop over them, if there is a hit - we return
    }
    return info;
}

}

