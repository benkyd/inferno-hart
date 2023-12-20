#include "object.hpp"

#include "graphics/device.hpp"
#include "mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <yolo/yolo.hpp>

namespace inferno::scene {

SceneObject* scene_object_create(graphics::GraphicsDevice* device)
{
    SceneObject* object = new SceneObject;
    object->Device = device;
    return object;
}

void scene_object_cleanup(SceneObject* object) { }

void processNode(SceneObject* object, const aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        Mesh* mesh = mesh_create(object->Device);
        mesh_process(mesh, scene->mMeshes[node->mMeshes[i]]);
        mesh_ready(mesh);
        scene_object_add_mesh(object, mesh);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(object, node->mChildren[i], scene);
    }
}

void scene_object_load(SceneObject* object, std::filesystem::path file)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(file.string(),
        aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph
            | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        yolo::error("Assimp error: {}", importer.GetErrorString());
        return;
    }

    processNode(object, scene->mRootNode, scene);
}

void scene_object_add_mesh(SceneObject* object, Mesh* mesh)
{
    object->Meshs.push_back(mesh);
}

std::vector<Mesh*>& scene_object_get_meshs(SceneObject* object) { return object->Meshs; }

}
