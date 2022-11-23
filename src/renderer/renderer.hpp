#pragma once

#include <graphics.hpp>

namespace inferno {

class HHM;

class Scene;
class RaySource;

class RayRenderer 
{
public:
    RayRenderer(HHM* accelIface);
    ~RayRenderer();

    void setScene(Scene* scene);

    void setTargetSize(glm::ivec2 size);
    glm::ivec2 getTargetSize();
    GLuint getRenderedTexture();

    void prepare();
    void draw();

private:
    GLuint mRenderTargetTexture = 0;
    
    glm::fvec4* mTarget;
    glm::ivec2 mRenderTargetSize = {300, 300};

    Scene* mCurrentScene = nullptr;
    RaySource* mRaySource;

private:
    HHM* mIface;
};

}
