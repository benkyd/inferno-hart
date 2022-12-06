#pragma once

#include <graphics.hpp>

#include <thread>
#include <mutex>
#include <atomic>

namespace inferno {

// Thread dispatcher keeps track of the thread that calls the
// copy function because OpenGL is a nightmare

class HHM;
class Scene;
class RayRenderer;

class RenderDispatcher 
{
public:
    RenderDispatcher();
    ~RenderDispatcher();

    RayRenderer* getRenderer();
    HHM* getTopModule();

    void startProgression();
    void stopProgression();
    bool progressionStatus();
    GLuint getLatestTexture();
    
private:
    HHM* mHHM;
    RayRenderer* mRenderer;

private:
    std::thread mRenderWorker;
    std::atomic<bool> mDoWork = false;
    std::atomic<bool> mJoin = false;
};

}
