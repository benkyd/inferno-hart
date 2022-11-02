#pragma once

#include <vector>
#include <filesystem>

#include <hart_graphics.hpp>

namespace inferno {

class ObjLoader;
class Material;

struct Vert
{
    glm::vec3 Position;
    glm::vec3 Normal;
    // glm::vec2 UV;
};

class Mesh 
{
public:
    Mesh();
    ~Mesh();

    void loadOBJ(std::filesystem::path file);
    void ready();

    void getVerticies(std::vector<glm::vec3>* vert,
                      std::vector<glm::vec3>* norm,
                      std::vector<glm::vec2>* uv);

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
