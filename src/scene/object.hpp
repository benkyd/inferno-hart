#pragma once

#include <memory>
#include <vector>
#include <filesystem>

namespace inferno::graphics {
struct GraphicsDevice;
struct Buffer;
}

namespace inferno::scene {

class Mesh;

typedef struct SceneObject {
    graphics::GraphicsDevice* Device;

    std::vector<Mesh*> Meshs;
} SceneObject;

SceneObject* scene_object_create(graphics::GraphicsDevice* device);
void scene_object_cleanup(SceneObject* object);

void scene_object_load(SceneObject* object, std::filesystem::path file);
void scene_object_optimize(SceneObject* object);

void scene_object_add_mesh(SceneObject* object, Mesh* mesh);
std::vector<Mesh*>& scene_object_get_meshs(SceneObject* object);

} // namespace inferno::scene
