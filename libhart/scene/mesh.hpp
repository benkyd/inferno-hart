#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>

namespace inferno {

class Mesh 
{
public:
    Mesh();
    ~Mesh();

    void loadOBJ();
    void ready();

    void getVerticies(std::vector<glm::vec3>* vert,
                      std::vector<glm::vec3>* norm,
                      std::vector<glm::vec2>* uv);

// Raster
public:
    GLuint getVAO();
    GLuint getVBO();

private:
    GLuint mVAO;
    GLuint mVBO;
    glm::mat4 mModel;

private:
    std::vector<glm::vec3> mVerticies;
    std::vector<glm::vec3> mNormals;
    std::vector<glm::vec2> mUVs;

};

}
