#pragma once

#include <vector>
#include <thirdparty/glm/glm.hpp>

namespace inferno {

class Mesh 
{
public:
    Mesh();
    ~Mesh();

private:
    std::vector<glm::vec3> mVerticies;
    std::vector<glm::vec3> mNormals;
    std::vector<glm::vec2> mUVs;

};

}
