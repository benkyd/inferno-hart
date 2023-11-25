#pragma once

#include <hart_graphics.hpp>

#include <array>
#include <filesystem>
#include <vector>

namespace inferno::scene {

class ObjLoader;
class Material;

// TODO: This should be procedural like everything else
typedef struct Vert {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 UV;
} Vert;

VkVertexInputBindingDescription get_vert_binding_description();
std::array<VkVertexInputAttributeDescription, 3> get_vert_attribute_descriptions();

typedef struct Mesh {
    Material* Material;
    glm::mat4 ModelMatrix;

    ObjLoader* ObjLoader;
    std::vector<Vert> Verticies;
} Mesh;

Mesh* mesh_create();
void mesh_cleanup(Mesh* mesh);

void mesh_load_obj(Mesh* mesh, std::filesystem::path file);
void mesh_ready(Mesh* mesh);

uint32_t mesh_get_verticies(Mesh* mesh, const float** v, const float** n);
uint32_t mesh_get_indicies(Mesh* mesh, const uint32_t** i);
uint32_t mesh_get_index_count(Mesh* mesh);

void mesh_set_model_matrix(Mesh* mesh, glm::mat4 model);
glm::mat4 mesh_get_model_matrix(Mesh* mesh);

void mesh_set_material(Mesh* mesh, Material* mat);
Material* mesh_get_material(Mesh* mesh);

}
