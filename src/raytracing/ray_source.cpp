#include "ray_source.hpp"

#include <iostream>

#include <maths.hpp>

#include <scene/camera.hpp>

#include <raytracing/ray.hpp>

namespace inferno::graphics::rays {

ReferencedRayField generate_initial_rays(graphics::Camera* camera, bool MSAA)
{
    auto viewport = graphics::camera_ray_get_viewport(camera);

    RayField field;
    field.reserve(viewport.x * viewport.y);

    const float aspect = static_cast<float>(viewport.x) / static_cast<float>(viewport.y);
    float scale = tan(camera->FOV / 2.0f * helpers::PI / 180.0f);

    glm::mat4 cameraToWorld = graphics::camera_get_look(camera);
    glm::vec3 origin = camera->Position;

    std::unordered_map<uint32_t, glm::ivec2> reference;

    uint32_t i = 0;
    for (int x = 0; x < viewport.x; x++)
        for (int y = 0; y < viewport.y; y++) {
            float Px = (2.0f * ((x + 0.5f) / viewport.x) - 1.0f) * scale * aspect;
            float Py = (2.0f * ((y + 0.5f) / viewport.y) - 1.0f) * scale;

            Ray* ray = new Ray {};

            glm::vec4 dir4 = glm::vec4(Px, Py, -1.0f, 1.0f) * cameraToWorld;
            glm::vec3 dir3 = glm::vec3(dir4) / dir4.w;
            ray->Direction = glm::normalize(dir3);

            ray->Origin = origin;
            ray->Reference = i;
            reference[i] = { x, y };

            field.push_back(ray);
            i++;
        }

    return { field, reference };
}

} // namespace inferno::graphics::rays
