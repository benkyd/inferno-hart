#include "mesh.hpp"

#include <scene/objloader.hpp>

#include "graphics/buffer.hpp"
#include "graphics/device.hpp"

#include <yolo/yolo.hpp>

#include <iostream>

namespace inferno::scene {

VkVertexInputBindingDescription get_vert_binding_description()
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vert);
    bindingDescription.inputRate
        = VK_VERTEX_INPUT_RATE_VERTEX; // VK_VERTEX_INPUT_RATE_INSTANCE

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3> get_vert_attribute_descriptions()
{
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

    // Position
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format
        = VK_FORMAT_R32G32B32_SFLOAT; // VK_FORMAT_R32G32B32A32_SFLOAT
    attributeDescriptions[0].offset = offsetof(Vert, Position);

    // Normal
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format
        = VK_FORMAT_R32G32B32_SFLOAT; // VK_FORMAT_R32G32B32A32_SFLOAT
    attributeDescriptions[1].offset = offsetof(Vert, Normal);

    // UV
    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format
        = VK_FORMAT_R32G32_SFLOAT; // VK_FORMAT_R32G32B32A32_SFLOAT
    attributeDescriptions[2].offset = offsetof(Vert, UV);

    return attributeDescriptions;
}

Mesh* mesh_create(graphics::GraphicsDevice* device)
{
    Mesh* mesh = new Mesh();
    mesh->Device = device;

    return mesh;
}

void mesh_cleanup(Mesh* mesh)
{
    delete mesh->MeshObjLoader;
    delete mesh;
}

void mesh_load_obj(Mesh* mesh, std::filesystem::path file)
{
    mesh->MeshObjLoader = new ObjLoader();
    mesh->MeshObjLoader->load(file);

    int vertCount = mesh->MeshObjLoader->getVertCount();
    for (int i = 0; i < vertCount * 3; i += 3) {
        Vert vert;
        vert.Position = {
            mesh->MeshObjLoader->getPositions()[i],
            mesh->MeshObjLoader->getPositions()[i + 1],
            mesh->MeshObjLoader->getPositions()[i + 2],
        };
        vert.Normal = {
            mesh->MeshObjLoader->getNormals()[i],
            mesh->MeshObjLoader->getNormals()[i + 1],
            mesh->MeshObjLoader->getNormals()[i + 2],
        };

        mesh->Verticies.push_back(vert);
    }
}

void mesh_ready(Mesh* mesh)
{
    void* data = mesh->Verticies.data();
    uint32_t size = mesh->Verticies.size() * sizeof(Vert);
    mesh->VertexBuffer = graphics::vertex_buffer_create(mesh->Device, data, size);

    yolo::debug("Mesh for preview ready...");
}

uint32_t mesh_get_verticies(Mesh* mesh, const float** v, const float** n)
{
    *v = &mesh->MeshObjLoader->getPositions()[0];
    *n = &mesh->MeshObjLoader->getNormals()[0];
    return mesh->MeshObjLoader->getVertCount();
}

uint32_t mesh_get_indicies(Mesh* mesh, const uint32_t** i)
{
    *i = &mesh->MeshObjLoader->getFaces()[0];
    return mesh->MeshObjLoader->getIndexCount();
}

uint32_t mesh_get_index_count(Mesh* mesh) { return mesh->MeshObjLoader->getIndexCount(); }

void mesh_set_model_matrix(Mesh* mesh, glm::mat4 model) { mesh->ModelMatrix = model; }

glm::mat4 mesh_get_model_matrix(Mesh* mesh) { return mesh->ModelMatrix; }

}
