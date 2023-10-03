#include "object.hpp"

#include "mesh.hpp"

namespace inferno::scene {

std::unique_ptr<SceneObject> scene_object_create()
{
    std::unique_ptr<SceneObject> object = std::make_unique<SceneObject>();
    return object;
}

void scene_object_cleanup(std::unique_ptr<SceneObject>& object)
{

}

void scene_object_add_mesh(std::unique_ptr<SceneObject>& object, Mesh* mesh)
{
    object->Meshs.push_back(mesh);
}

std::vector<Mesh*>& scene_object_get_meshs(std::unique_ptr<SceneObject>& object)
{
    return object->Meshs;
}

}

