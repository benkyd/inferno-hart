#pragma once

#include <memory>
#include <vector>

namespace inferno::scene {

class Mesh;

typedef struct SceneObject {
    std::vector<Mesh*> Meshs;
} SceneObject;

std::unique_ptr<SceneObject> scene_object_create();
void scene_object_cleanup(std::unique_ptr<SceneObject>& object);

void scene_object_add_mesh(std::unique_ptr<SceneObject>& object, Mesh* mesh);
std::vector<Mesh*>& scene_object_get_meshs(std::unique_ptr<SceneObject>& object);

} // namespace inferno::scene
