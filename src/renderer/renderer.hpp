#pragma once

#include <graphics.hpp>

#include <mutex>
#include <unordered_map>
#include <condition_variable>

namespace inferno {

class HHM;

class Scene;
class HitInfo;
class RaySource;
class RenderDispatcher;

class RayRenderer
{
public:
    RayRenderer(HHM* accelIface);
    ~RayRenderer();

    void setScene(Scene* scene);

    void setTargetSize(glm::ivec2 size);
    glm::ivec2 getTargetSize();
    GLuint getRenderedTexture();
    glm::fvec4* getRenderData();

    void prepare();
    void draw();

public:
    void computeHit(HitInfo* info);

private:
    void mHaultWait();

    std::unordered_map<uint32_t, glm::ivec2>* mCurrentRefTable;

private:
    GLuint mRenderTargetTexture = 0;
    glm::fvec4* mTarget;

    std::mutex _RenderData;
    std::condition_variable _RenderPause;

    glm::ivec2 mRenderTargetSize = {200, 200};

    Scene* mCurrentScene = nullptr;
    RaySource* mRaySource = nullptr;

    friend class RenderDispatcher;
private:
    HHM* mIface;
};

}
