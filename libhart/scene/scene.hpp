#pragma once

#include <vector>

namespace inferno {

class SceneObject;
class Camera;
class Mesh;
class Sky;

class Scene 
{
public:
    Scene();
    ~Scene();

    void setCamera(Camera* camera);
    Camera* getCamera();
    void addMesh(Mesh* mesh);

    const std::vector<Mesh*>& getRenderables();

private:
    std::vector<Mesh*> mMeshs;

    Camera* mCurrentCamera;
    Sky* mCurrentSky;
};

}
