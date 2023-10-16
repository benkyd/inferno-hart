#pragma once

#include <memory>
#include <vector>

namespace inferno::scene {

class Mesh;

typedef struct SceneObject {
    std::vector<Mesh*> Meshs;
} SceneObject;

SceneObject* scene_object_create();
void scene_object_cleanup(SceneObject* object);

void scene_object_add_mesh(SceneObject* object, Mesh* mesh);
std::vector<Mesh*>& scene_object_get_meshs(SceneObject* object);

} // namespace inferno::scene
