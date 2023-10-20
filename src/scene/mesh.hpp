#pragma once
#include <vector>
#include <filesystem>

#include <hart_graphics.hpp>

namespace inferno::scene {

class ObjLoader;
class Material;

// TODO: This should be procedural like everything else
struct Vert
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 UV;
};

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void loadOBJ(std::filesystem::path file);
    void ready();

    int getVerticies(const float** v, const float** n);
    int getIndicies(const uint32_t** i);

    int getIndexCount();

    void setMaterial(Material* mat);
    Material* getMaterial();

// Raster
public:
    GLuint getVAO();
    GLuint getVBO();
    GLuint getEBO();

private:
    GLuint mVAO;
    GLuint mVBO;
    GLuint mEBO;
    glm::mat4 mModel;

private:
    ObjLoader* mObjLoader;
    Material* mMaterial;

    std::vector<Vert> mVerticies;
};

}
