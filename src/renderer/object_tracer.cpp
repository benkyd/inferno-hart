#include "object_tracer.hpp"
#include "scene/object.hpp"

#include <cmath>
#include <maths.hpp>
#include <yolo/yolo.hpp>

namespace inferno::graphics::rays {

bool triangle_ray_collide(Ray* ray, float* t, glm::vec3 vertex0, glm::vec3 vertex1, glm::vec3 vertex2)
{
    glm::vec3 edge1, edge2, h, s, q;
    float a, f, u, v;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;
    h = glm::cross(ray->Direction, edge2);
    a = glm::dot(edge1, h);
    if (a > -helpers::EPSILON && a < helpers::EPSILON)
        return false; // This ray is parallel to this triangle.
    f = 1.0 / a;
    s = ray->Origin - vertex0;
    u = f * glm::dot(s, h);
    if (u < 0.0 || u > 1.0)
        return false;
    q = glm::cross(s, edge1);
    v = f * glm::dot(ray->Direction, q);
    if (v < 0.0 || u + v > 1.0)
        return false;
    // At this stage we can compute t to find out where the intersection point is on the line.
    *t = f * glm::dot(edge2, q);
    if (*t > helpers::EPSILON) // ray intersection
        return true;
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}

HitInfo* object_ray_collide(scene::SceneObject* object, Ray* ray)
{
    HitInfo* info = nullptr;
    for (auto* mesh : scene::scene_object_get_meshs(object)) {
        // extract triangles and loop over them, if there is a hit - we return
        const uint32_t* ind;
        const float* verts;
        const float* norms;
        mesh->getIndicies(&ind);
        mesh->getVerticies(&verts, &norms);

        float t = INFINITY;
        for (int i = 0; i < mesh->getIndexCount(); i += 3) {
            uint32_t index = ind[i];
            glm::vec3 a = {  };
            glm::vec3 b = {  };
            glm::vec3 c = {  };
        }
    }
    return info;
}

}
