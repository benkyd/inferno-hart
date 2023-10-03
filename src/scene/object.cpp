#include "object.hpp"

#include "mesh.hpp"

namespace inferno::scene {

SceneObject* scene_object_create()
{
    SceneObject* object = new SceneObject;
    return object;
}

void scene_object_cleanup(SceneObject* object)
{

}

void scene_object_add_mesh(SceneObject* object, Mesh* mesh)
{
    object->Meshs.push_back(mesh);
}

std::vector<Mesh*>& scene_object_get_meshs(SceneObject* object)
{
    return object->Meshs;
}

}

