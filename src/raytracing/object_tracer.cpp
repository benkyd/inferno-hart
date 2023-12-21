#include "object_tracer.hpp"

#include "preview/debug.hpp"
#include "scene/object.hpp"

#include <cmath>
#include <maths.hpp>
#include <yolo/yolo.hpp>

namespace inferno::graphics::rays {

bool triangle_ray_collide(Ray* ray, float* t, glm::vec3 vertex0, glm::vec3 vertex1, glm::vec3 vertex2)
{
    debug_draw_line(vertex0, vertex1, { 1, 0, 0 });
    debug_draw_line(vertex1, vertex2, { 1, 0, 0 });
    debug_draw_line(vertex2, vertex0, { 1, 0, 0 });

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
    HitInfo* info = new HitInfo;
    info->Distance = INFINITY;
    info->Did = false;
    for (auto* mesh : scene::scene_object_get_meshs(object)) {
        // extract triangles and loop over them, if there is a hit - we return
        const uint32_t* ind;
        const float* verts;
        const float* norms;
        // mesh->getIndicies(&ind);
        // mesh->getVerticies(&verts, &norms);

        float t = INFINITY;
        int i ;
        // for (int i = 0; i < mesh->getIndexCount(); i += 3) {
            uint32_t indexa = ind[i + 0];
            uint32_t indexb = ind[i + 1];
            uint32_t indexc = ind[i + 2];
            glm::vec3 a = { verts[3 * indexa + 0], verts[3 * indexa + 1], verts[3 * indexa + 2] };
            glm::vec3 b = { verts[3 * indexb + 0], verts[3 * indexb + 1], verts[3 * indexb + 2] };
            glm::vec3 c = { verts[3 * indexc + 0], verts[3 * indexc + 1], verts[3 * indexc + 2] };

            float temp_t;
            if (!triangle_ray_collide(ray, &temp_t, a, b, c))
                continue;

            info->Did = true;
            if (temp_t < t) {
                t = temp_t;
                info->Distance = t;
            }
        // }
    }
    return info;
}

}
