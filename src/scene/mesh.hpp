#pragma once

#include <hart_graphics.hpp>

#include <array>
#include <filesystem>
#include <vector>

namespace inferno::graphics {
struct GraphicsDevice;
struct Buffer;
}

namespace inferno::scene {

class ObjLoader;

// TODO: This should be procedural like everything else
struct Vert {
    glm::vec3 Position;
    glm::vec3 Normal;
};

struct DebugLineVert {
    glm::vec3 Position;
    glm::vec3 Color;
};

using Index = uint32_t;

// NOTE: Out of coincidence, this is the same for both vert and debug line vert
// TODO: Make this a template // somehow
VkVertexInputBindingDescription get_vert_binding_description();
std::array<VkVertexInputAttributeDescription, 2> get_vert_attribute_descriptions();

typedef struct Mesh {
    graphics::GraphicsDevice* Device;

    glm::mat4 ModelMatrix;

    ObjLoader* MeshObjLoader;
    std::vector<Vert> Verticies;

    graphics::Buffer* VertexBuffer;
    graphics::Buffer* IndexBuffer;
} Mesh;

Mesh* mesh_create(graphics::GraphicsDevice* device);
void mesh_cleanup(Mesh* mesh);

void mesh_load_obj(Mesh* mesh, std::filesystem::path file);
void mesh_ready(Mesh* mesh);

uint32_t mesh_get_verticies(Mesh* mesh, const float** v, const float** n);
uint32_t mesh_get_indicies(Mesh* mesh, const uint32_t** i);
uint32_t mesh_get_index_count(Mesh* mesh);

void mesh_set_model_matrix(Mesh* mesh, glm::mat4 model);
glm::mat4 mesh_get_model_matrix(Mesh* mesh);

}
