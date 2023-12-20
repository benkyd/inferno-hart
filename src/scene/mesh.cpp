#include "mesh.hpp"

#include "graphics/buffer.hpp"
#include "graphics/device.hpp"

#include <yolo/yolo.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <iostream>

namespace inferno::scene {

VkVertexInputBindingDescription get_vert_binding_description()
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vert);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> get_vert_attribute_descriptions()
{
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

    // Position
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vert, Position);

    // Normal
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vert, Normal);

    return attributeDescriptions;
}

Mesh* mesh_create(graphics::GraphicsDevice* device)
{
    Mesh* mesh = new Mesh();
    mesh->Device = device;

    return mesh;
}

void mesh_cleanup(Mesh* mesh) { delete mesh; }

void mesh_process(Mesh* out, aiMesh* mesh)
{
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vert vertex = {
            .Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
            .Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z),
        };
        out->Verticies.push_back(vertex);
    }

    // indicies
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            out->Indicies.push_back(face.mIndices[j]);
        }
    }
}

void mesh_ready(Mesh* mesh)
{
    void* data = mesh->Verticies.data();
    void* indexData = mesh->Indicies.data();

    uint32_t size = mesh->Verticies.size();
    yolo::debug("Mesh size: {}", size);

    mesh->VertexBuffer = graphics::vertex_buffer_create(mesh->Device, data, size);
    mesh->IndexBuffer
        = graphics::index_buffer_create(mesh->Device, indexData, mesh->Indicies.size());

    yolo::debug("Mesh for preview ready...");
}

// TODO: Extract vertex and normal data from internal mesh
uint32_t mesh_get_verticies(Mesh* mesh, const float** v, const float** n)
{
    return mesh->Verticies.size();
}

uint32_t mesh_get_indicies(Mesh* mesh, const uint32_t** i)
{
    *i = mesh->Indicies.data();
    return mesh_get_index_count(mesh);
}

uint32_t mesh_get_index_count(Mesh* mesh) { return mesh->Indicies.size(); }

void mesh_set_model_matrix(Mesh* mesh, glm::mat4 model) { mesh->ModelMatrix = model; }

glm::mat4 mesh_get_model_matrix(Mesh* mesh) { return mesh->ModelMatrix; }

}
