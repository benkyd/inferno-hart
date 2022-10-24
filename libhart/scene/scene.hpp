#pragma once

#include <vector>

namespace inferno {

class SceneObject;
class Sky;

class Scene 
{
public:
    Scene();
    ~Scene();

    SceneObject* newObject();

private:
    std::vector<SceneObject*> mObjects;
    Sky* mCurrentSky;
};

}
