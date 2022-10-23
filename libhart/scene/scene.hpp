#pragma once

#include <vector>

namespace core::rendering {

class SceneObject;

class Scene 
{
public:
    Scene();
    ~Scene();

    SceneObject* newObject();

private:
    std::vector<SceneObject*> mObjects;
};

}
