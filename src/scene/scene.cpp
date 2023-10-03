#include "scene.hpp"

#include <memory>
#include <scene/camera.hpp>
#include <scene/mesh.hpp>
#include <scene/object.hpp>

namespace inferno::scene {

std::unique_ptr<Scene> scene_create()
{
    std::unique_ptr<Scene> scene = std::make_unique<Scene>();
    scene->Camera = std::make_unique<graphics::Camera>();
    return scene;
}

void scene_cleanup(std::unique_ptr<Scene>& scene)
{
}

void scene_add_object(std::unique_ptr<Scene>& scene, std::unique_ptr<SceneObject>& object)
{
    scene->Objects.emplace_back(std::move(object));
    scene->DidUpdate = true;
}

std::unique_ptr<graphics::Camera>& scene_get_camera(std::unique_ptr<Scene>& scene)
{
    return scene->Camera;
}

std::vector<std::unique_ptr<SceneObject>>& scene_get_renderables(std::unique_ptr<Scene>& scene)
{
    return scene->Objects;
}

bool scene_did_update(std::unique_ptr<Scene>& scene)
{
    return scene->DidUpdate;
}

void scene_frame_tick(std::unique_ptr<Scene>& scene)
{
    scene->DidUpdate = false;
}

void scene_tick(std::unique_ptr<Scene>& scene)
{
    for (auto& object : scene->Objects) {
        // Shit here like animation idk
    }
}

}
