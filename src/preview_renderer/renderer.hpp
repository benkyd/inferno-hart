#pragma once

namespace inferno {

class Camera;
class Scene;

class RasterizeRenderer 
{
public:
    RasterizeRenderer();
    ~RasterizeRenderer();

    void setScene(Scene* scene);

    void prepare();
    void pass();

private:
    Camera* mCurrentCamera;
    Scene* mCurrentScene;
};

}
