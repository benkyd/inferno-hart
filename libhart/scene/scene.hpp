#pragma once

#include <vector>

namespace inferno {

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
