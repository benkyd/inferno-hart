#include "scene.hpp"

#include <yolo/yolo.hpp>

#include <memory>
#include <scene/camera.hpp>
#include <scene/mesh.hpp>
#include <scene/object.hpp>

namespace inferno::scene {

Scene* scene_create()
{
    Scene* scene = new Scene;
    scene->Objects = std::vector<SceneObject*>();
    scene->Camera = graphics::camera_create();
    yolo::debug("Created scene {} and camera {}", scene, scene->Camera);
    return scene;
}

void scene_cleanup(Scene* scene)
{
}

void scene_add_object(Scene* scene, SceneObject* object)
{
    yolo::debug("Using scene {}", scene);
    yolo::debug("Adding object to scene, no Objects: {}, adding to pool of: {}", object->Meshs.size(), scene->Objects.size());
    scene->Objects.push_back(object);
    scene->DidUpdate = true;
}

graphics::Camera* scene_get_camera(Scene* scene)
{
    return scene->Camera;
}

std::vector<SceneObject*>& scene_get_renderables(Scene* scene)
{
    return scene->Objects;
}

bool scene_did_update(Scene* scene)
{
    return scene->DidUpdate;
}

void scene_frame_tick(Scene* scene)
{
    scene->DidUpdate = false;
}

void scene_tick(Scene* scene)
{
    for (auto& object : scene->Objects) {
        // Shit here like animation idk
    }
}

}
