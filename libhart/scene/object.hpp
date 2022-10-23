#pragma once

#include <vector>

namespace core::rendering {

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
