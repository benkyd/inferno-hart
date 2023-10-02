#pragma once

#include <memory>
#include <vector>

namespace inferno::graphics {
class Camera;
}

namespace inferno::scene {

class SceneObject;
class Mesh;

typedef struct Scene {
    std::unique_ptr<graphics::Camera> Camera;
    std::vector<std::unique_ptr<SceneObject>> Objects;
    bool DidUpdate = false;
} Scene;

std::unique_ptr<Scene> scene_create();
void scene_cleanup(std::unique_ptr<Scene>& scene);
void scene_add_object(std::unique_ptr<Scene>& scene, std::unique_ptr<SceneObject> object);
std::unique_ptr<graphics::Camera>& scene_get_camera(std::unique_ptr<Scene>& scene);
std::vector<std::unique_ptr<SceneObject>>& scene_get_renderables(std::unique_ptr<Scene>& scene);
bool scene_did_update(std::unique_ptr<Scene>& scene);
void scene_frame_tick(std::unique_ptr<Scene>& scene);
void scene_tick(std::unique_ptr<Scene>& scene);

}
