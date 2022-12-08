#pragma once

#include <vector>
#include <filesystem>

#include <hart_graphics.hpp>

namespace inferno {

struct Face
{
    int a;
    int b;
    int c;
};

class ObjLoader
{
public:
    ObjLoader();
    ~ObjLoader();

    void load(std::filesystem::path file);

    int getIndexCount();
    int getVertCount();

    const uint32_t* getFaces();
    const float* getPositions();
    const float* getNormals();

    uint32_t getTexCoordLayers();
    const float* getTexCoords(int multiTexCoordLayer);
private:

    std::vector<Face> mFaces;
    std::vector<glm::vec3> mPositions;
    std::vector<glm::vec3> mNormals;
    // obj's only have 1 layer ever
    std::vector<glm::vec2> mTexCoords;
    uint32_t mTexCoordLayers;
};

}
