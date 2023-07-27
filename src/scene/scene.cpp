#include "scene.hpp"

#include <scene/camera.hpp>
#include <scene/mesh.hpp>
#include <scene/object.hpp>

namespace inferno::scene {

std::unique_ptr<Scene> scene_create()
{
    std::unique_ptr<Scene> scene = std::make_unique<Scene>();
    scene->Camera = std::make_shared<graphics::Camera>();
    return scene;
}

void scene_cleanup(std::unique_ptr<Scene>& scene)
{
}

void scene_set_camera(std::unique_ptr<Scene>& scene, std::shared_ptr<graphics::Camera> camera)
{
    scene->Camera = camera;
    scene->DidUpdate = true;
}

void scene_add_object(std::unique_ptr<Scene>& scene, std::unique_ptr<SceneObject> object)
{
    scene->Objects.push_back(std::move(object));
    scene->DidUpdate = true;
}

std::vector<std::unique_ptr<SceneObject>>& get_renderables(std::unique_ptr<Scene>& scene)
{
    return scene->Objects;
}

bool did_update(std::unique_ptr<Scene>& scene)
{
    return scene->DidUpdate;
}

void frame_tick(std::unique_ptr<Scene>& scene)
{
    scene->DidUpdate = false;
}

void tick(std::unique_ptr<Scene>& scene)
{
    for (auto& object : scene->Objects) {
        // object->tick();
    }
}

}

