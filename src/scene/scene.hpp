#pragma once

#include "hart_graphics.hpp"

#include "scene/object.hpp"

#include <memory>
#include <vector>

namespace inferno::graphics {
class Camera;
}

namespace inferno::scene {

class SceneObject;
class Mesh;

typedef struct GlobalUniformObject {
    glm::mat4 Projection;
    glm::mat4 View;
    glm::mat4 _padding0; // Fuck you nvidia!
    glm::mat4 _padding1;
} GlobalUniformObject;

typedef struct Scene {
    std::vector<SceneObject*> Objects;
    graphics::Camera* Camera;
    bool DidUpdate = false;
} Scene;

Scene* scene_create();
void scene_cleanup(Scene* scene);

void scene_add_object(Scene* scene, SceneObject* object);

graphics::Camera* scene_get_camera(Scene* scene);
std::vector<SceneObject*>& scene_get_renderables(Scene* scene);

bool scene_did_update(Scene* scene);

void scene_frame_tick(Scene* scene);
void scene_tick(Scene* scene);

} // namespace inferno::scene
