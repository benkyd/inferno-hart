#include "mesh.hpp"

#include <yolo/yolo.hpp>

#include <scene/objloader.hpp>

#include <iostream>

namespace inferno::scene {

VkVertexInputBindingDescription get_vert_binding_description()
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vert);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // VK_VERTEX_INPUT_RATE_INSTANCE

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3> get_vert_attribute_descriptions()
{
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

    // Position
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT; // VK_FORMAT_R32G32B32A32_SFLOAT
    attributeDescriptions[0].offset = offsetof(Vert, Position);

    // Normal
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT; // VK_FORMAT_R32G32B32A32_SFLOAT
    attributeDescriptions[1].offset = offsetof(Vert, Normal);

    // UV
    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT; // VK_FORMAT_R32G32B32A32_SFLOAT
    attributeDescriptions[2].offset = offsetof(Vert, UV);

    return attributeDescriptions;
}

Mesh* mesh_create()
{
    Mesh* mesh = new Mesh();
    return mesh;
}

void mesh_cleanup(Mesh* mesh)
{
    delete mesh->ObjLoader;
    delete mesh;
}

void mesh_load_obj(Mesh* mesh, std::filesystem::path file)
{
    mesh->ObjLoader = new ObjLoader();
    mesh->ObjLoader->load(file);

    int vertCount = mesh->ObjLoader->getVertCount();
    for (int i = 0; i < vertCount * 3; i += 3) {
        Vert vert;
        vert.Position = {
            mesh->ObjLoader->getPositions()[i],
            mesh->ObjLoader->getPositions()[i + 1],
            mesh->ObjLoader->getPositions()[i + 2],
        };
        vert.Normal = {
            mesh->ObjLoader->getNormals()[i],
            mesh->ObjLoader->getNormals()[i + 1],
            mesh->ObjLoader->getNormals()[i + 2],
        };

        mesh->Verticies.push_back(vert);
    }
}

void mesh_ready(Mesh* mesh) { yolo::debug("Mesh for preview ready..."); }

uint32_t mesh_get_verticies(Mesh* mesh, const float** v, const float** n)
{
    *v = &mesh->ObjLoader->getPositions()[0];
    *n = &mesh->ObjLoader->getNormals()[0];
    return mesh->ObjLoader->getVertCount();
}

uint32_t mesh_get_indicies(Mesh* mesh, const uint32_t** i)
{
    *i = &mesh->ObjLoader->getFaces()[0];
    return mesh->ObjLoader->getIndexCount();
}

uint32_t mesh_get_index_count(Mesh* mesh) { return mesh->ObjLoader->getIndexCount(); }

void mesh_set_model_matrix(Mesh* mesh, glm::mat4 model) { mesh->ModelMatrix = model; }

glm::mat4 mesh_get_model_matrix(Mesh* mesh) { return mesh->ModelMatrix; }

void mesh_set_material(Mesh* mesh, Material* mat) { mesh->Material = mat; }

Material* mesh_get_material(Mesh* mesh) { return mesh->Material; }

}
