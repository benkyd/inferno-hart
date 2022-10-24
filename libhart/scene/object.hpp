#pragma once

#include <vector>

namespace inferno {

class Mesh;

class Object 
{
public:
    Object();
    ~Object();

private:
    std::vector<Mesh*> mMeshs;
};

}
